/******************************************************************************

                        QMI_IP_MSG_HDLR.C

******************************************************************************/

/******************************************************************************

  @file    qmi_ip_msg_hdlr.c
  @brief   Qualcomm mapping interface over IP Message handler

  DESCRIPTION


  ---------------------------------------------------------------------------
  Copyright (c) 2013 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
  ---------------------------------------------------------------------------


******************************************************************************/

/******************************************************************************

                      EDIT HISTORY FOR FILE

  $Id:$

when       who        what, where, why
--------   ---        -------------------------------------------------------
08/30/13   tw         Initial version

******************************************************************************/

/*===========================================================================
                              INCLUDE FILES
===========================================================================*/

#include "qmi_ip.h"

#define QMI_PLATFORM_NUM_CHARS_PER_BYTE (3) /* 2 hex chars per byte + space */
#define QMI_PLATFORM_NUM_BYTES_PER_LINE (16)

#define QMI_PLATFORM_GET_HEX_CHAR(x)  \
  (((x) > 0x0F) ? '*' : hex_digit_to_char_tbl[(x)])

static const char hex_digit_to_char_tbl[] =
{
  '0', '1', '2', '3', '4', '5', '6', '7',
  '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'
};

/*===========================================================================
                              FUNCTION DEFINITIONS
===========================================================================*/

int proccess_nas_message(unsigned char**, unsigned long, int, unsigned long tx_id);
void disable_embms(void);

/*===========================================================================
  FUNCTION  DISABLE_EMBMS
===========================================================================*/
/*!
@brief

  - Disable the eMBMS connection

@return

@note

  - Dependencies
    TCP connection to GW IP
  - Side Effects
    - None
*/
/*=========================================================================*/
void disable_embms(void){
  int ret;
  nas_config_embms_req_msg_v01 req_msg;
  nas_config_embms_resp_msg_v01 resp_msg;

  memset(&req_msg, 0, sizeof(nas_config_embms_req_msg_v01));
  req_msg.enable = 0;

  ret = qmi_client_send_msg_sync(qmi_nas_handle,
                                 QMI_NAS_CONFIG_EMBMS_REQ_MSG_V01,
                                 &req_msg,
                                 sizeof(req_msg),
                                 &resp_msg,
                                 sizeof(resp_msg),
                                 QMI_TIMEOUT);

  LOG_MSG_ERROR("Config eMBMS req, error % result %d", ret,
                resp_msg.resp.result, 0);

  if (ret != QMI_NO_ERR || resp_msg.resp.result != QMI_NO_ERR)
  {
    LOG_MSG_ERROR("Failed to disconnect eMBMS call, error %d %d", ret,
                  resp_msg.resp.error, 0);
  }
}

/*===========================================================================
  FUNCTION  PRINT_QMI_MSG
===========================================================================*/
/*!
@brief

  - Print the QMI message in hex

@return

@note

*/
/*=========================================================================*/
void print_qmi_msg (
  unsigned char           *msg,
  int                     msg_len
)
{
  int i, j;
  char buff[QMI_PLATFORM_NUM_CHARS_PER_BYTE*QMI_PLATFORM_NUM_BYTES_PER_LINE+1] = "";
  unsigned char upper_half;
  unsigned char lower_half;
  const unsigned char *data = msg;

  if (NULL == msg)
  {
    return;
  }

  for (i = 1, j = 0; i <= msg_len; ++i, ++data)
  {
    upper_half = (*data) >> 4;
    lower_half = (*data) & 0x0F;
    buff[j++]  = QMI_PLATFORM_GET_HEX_CHAR(upper_half);
    buff[j++]  = QMI_PLATFORM_GET_HEX_CHAR(lower_half);
    buff[j++]  = ' ';

    if (i % QMI_PLATFORM_NUM_BYTES_PER_LINE == 0)
    {
      buff[j] = '\0';
      LOG_MSG_ERROR ("%s\n", buff,0,0);
      j = 0;
    }
  }

  /* Print any remaining data */
  if (j > 0)
  {
    buff[j] = '\0';
    LOG_MSG_ERROR ("%s\n", buff,0,0);
  }
}

/*===========================================================================
  FUNCTION  PROCESS_WDS_MESSAGE
===========================================================================*/
/*!
@brief

  Proccess the wds messages from the GW.

@return

  - Calls corresponding dsi_netctrl function based off wds message

@note

  - Dependencies
    - TCP link to GW active

  - Side Effects
    - None
*/
/*=========================================================================*/

int proccess_wds_message(unsigned char **msg,
                         unsigned long message_id,
                         int msg_len,
                         unsigned long tx_id)
{
    int             ret;
    unsigned long   type;
    unsigned char   *value_ptr = NULL;
    unsigned char   temp_char;
    unsigned long   value_len;
    int             error_code = QMI_ERR_NONE_V01;
    int             tmp_msg_len;

    unsigned char *msg_ptr;
    unsigned char *tmp_msg_ptr;
    int tlv_len;
    unsigned char *tlv_msg_ptr;
    unsigned char *tlv_tmp_msg_ptr;
    int msg_ptr_len, resp_len;
    unsigned char *resp_msg_ptr;

    if(message_id == QMI_WDS_START_NETWORK_INTERFACE) {
        //if the network tech pref is anything but eMBMS do not forward it to modem
        tmp_msg_ptr = *msg;
        tmp_msg_len = msg_len;

        while (tmp_msg_len >0)
        {
            if (( ret = qmi_util_read_std_tlv(&tmp_msg_ptr, &tmp_msg_len, &type, &value_len,
                                              &value_ptr)) ==  QMI_INTERNAL_ERR){
                error_code = QMI_ERR_MISSING_ARG_V01;
                break;
            }

            if(type == QMI_WDS_NW_IF_XTENDED_TECH_PREF_REQ_TLV_ID){
                error_code = QMI_ERR_NONE_V01;
                break;
            }
            else
                error_code = QMI_ERR_INVALID_TECH_PREF_V01;
        }
    }
    else if ((message_id == QMI_WDS_EMBMS_TMGI_ACTIVATE || message_id == QMI_WDS_EMBMS_TMGI_ACT_DEACT)
             && embms_call_state && !netmgr_status) {
        error_code = QMI_ERR_INTERNAL_V01;
    }

    if (error_code == QMI_ERR_NONE_V01) {
        msg_ptr = *msg;
        ret =  qmi_client_send_raw_msg_sync (qmi_ip_conf.qmi_ip_v4_wds_handle, message_id,
                                            (unsigned char *)((unsigned char *)msg_ptr),
                                             msg_len, msg_ptr, QMI_MAX_STD_MSG_SIZE,
                                             &resp_len, QMI_TIMEOUT);
        if (ret == QMI_NO_ERR){
            msg_ptr_len = QMI_HEADER_SIZE + resp_len;
            resp_msg_ptr = malloc(msg_ptr_len);
            if (resp_msg_ptr == NULL){
                LOG_MSG_ERROR("MALLOC failure",0,0,0);
                return QMI_IP_ERROR;
            }
            tmp_msg_ptr = resp_msg_ptr;

            WRITE_QMI_HEADER(tmp_msg_ptr, msg_ptr_len, QMI_WDS_SERVICE_TYPE, message_id,
                             QMI_RESP_CTRL_FLAGS, tx_id, msg_ptr_len - QMI_HEADER_SIZE);
            memcpy(tmp_msg_ptr, msg_ptr, resp_len);

            ret = send(qmi_ip_server_fd, resp_msg_ptr, msg_ptr_len, 0);
            CHK_ERR(ret, "Send Failure");

            free(resp_msg_ptr);
            return TRUE;
        }
        else
            error_code = QMI_ERR_INTERNAL_V01;
    }

    //Something was wrong with the msg, send error back to GW
    msg_ptr_len = QMI_HEADER_SIZE + QMI_ERROR_SIZE;
    msg_ptr = malloc(msg_ptr_len);
    if (msg_ptr == NULL){
        LOG_MSG_ERROR("MALLOC failure",0,0,0);
        return QMI_IP_ERROR;
    }
    tmp_msg_ptr = msg_ptr;

    WRITE_QMI_HEADER(tmp_msg_ptr, msg_ptr_len, QMI_WDS_SERVICE_TYPE, message_id, QMI_RESP_CTRL_FLAGS, tx_id, msg_ptr_len - QMI_HEADER_SIZE);

    //write TLV
    tlv_len = QMI_ERROR_TLV;
    tlv_msg_ptr = malloc(tlv_len);
    if (tlv_msg_ptr == NULL){
        LOG_MSG_ERROR("MALLOC failure",0,0,0);
        free(msg_ptr);
        return QMI_IP_ERROR;
    }
    tlv_tmp_msg_ptr = tlv_msg_ptr;

    WRITE_16_BIT_VAL(tlv_tmp_msg_ptr, QMI_RESULT_FAILURE_V01);
    WRITE_16_BIT_VAL(tlv_tmp_msg_ptr, error_code);

    if (tlv_len > 0 && qmi_util_write_std_tlv(&tmp_msg_ptr, &msg_ptr_len, QMI_IP_RESULT_CODE, tlv_len, (void *)tlv_msg_ptr) < 0)
    {
        LOG_MSG_INFO2("write TLV failed",0,0,0);
    }

    ret = send(qmi_ip_server_fd, msg_ptr, msg_ptr_len, 0);
    CHK_ERR(ret, "Send failure");

    free(msg_ptr);
    free(tlv_msg_ptr);
    return TRUE;
}

/*===========================================================================
  FUNCTION  PROCESS_NAS_MESSAGE
===========================================================================*/
/*!
@brief

  Proccess the nas messages from the GW.

@return

  - Calls corresponding NAS function based off message

@note

  - Dependencies
    - TCP link to GW active

  - Side Effects
    - None
*/
/*=========================================================================*/

//needs to be made into generic function for any QMI service
int proccess_nas_message(unsigned char **msg,
                         unsigned long message_id,
                         int msg_len,
                         unsigned long tx_id)
{
  int            ret;
  unsigned char *msg_ptr;
  unsigned char *resp_msg_ptr;
  unsigned char *tmp_msg_ptr;
  unsigned char *tlv_msg_ptr;
  unsigned char *tlv_tmp_msg_ptr;
  int            msg_ptr_len;
  int            resp_len, tlv_len;

  msg_ptr = *msg;
  int *qmi_error_code;
  ret =  qmi_client_send_raw_msg_sync (qmi_nas_handle, message_id,
                                      (unsigned char *)((unsigned char *)msg_ptr),
                                       msg_len, msg_ptr, QMI_MAX_STD_MSG_SIZE,
                                       &resp_len, QMI_TIMEOUT);

  if (ret == QMI_NO_ERR)
  {
    msg_ptr_len = QMI_HEADER_SIZE + resp_len;
    resp_msg_ptr = malloc(msg_ptr_len);
    if (resp_msg_ptr == NULL){
        LOG_MSG_ERROR("MALLOC failure",0,0,0);
        return QMI_IP_ERROR;
    }
    tmp_msg_ptr = resp_msg_ptr;

    WRITE_QMI_HEADER(tmp_msg_ptr, msg_ptr_len, QMI_NAS_SERVICE_TYPE, message_id,
                     QMI_RESP_CTRL_FLAGS, tx_id, msg_ptr_len - QMI_HEADER_SIZE);

    memcpy(tmp_msg_ptr, msg_ptr, resp_len);
  }
  else
  {
    LOG_MSG_ERROR("qmi_client_send_raw_msg_sync error %d",ret,0,0);
    /* if qmi_client_send_raw_msg_sync fails we still need to send a
    response back to the GW */
    msg_ptr_len = QMI_HEADER_SIZE + 7;
    resp_msg_ptr = malloc(msg_ptr_len);

    if (resp_msg_ptr == NULL){
        LOG_MSG_ERROR("MALLOC failure",0,0,0);
        return QMI_IP_ERROR;
    }
    tmp_msg_ptr = resp_msg_ptr;

    WRITE_QMI_HEADER(tmp_msg_ptr, msg_ptr_len, QMI_NAS_SERVICE_TYPE,
                     message_id, QMI_RESP_CTRL_FLAGS, tx_id,
                     msg_ptr_len - QMI_HEADER_SIZE);

    tlv_len = QMI_ERROR_TLV;
    tlv_msg_ptr = malloc(tlv_len);
    if (tlv_msg_ptr == NULL){
        free(resp_msg_ptr);
        return QMI_IP_ERROR;
    }
    tlv_tmp_msg_ptr = tlv_msg_ptr;

    WRITE_16_BIT_VAL(tlv_tmp_msg_ptr, QMI_RESULT_FAILURE_V01);
    WRITE_16_BIT_VAL(tlv_tmp_msg_ptr, QMI_ERR_UNKNOWN_V01);

    if (qmi_util_write_std_tlv(&tmp_msg_ptr, &msg_ptr_len,
                               QMI_IP_RESULT_CODE, tlv_len,
                               (void *)tlv_msg_ptr) < 0)
    {
      LOG_MSG_INFO2("NAS return message write TLV failed",0,0,0);
    }

    free(tlv_msg_ptr);
  }

  /* Check is needed since a nas message is sent to disable embms when TCP
  goes down if dsi_net_mode is connected */
  if (qmi_ip_server_fd != 0) {
    ret = send(qmi_ip_server_fd, resp_msg_ptr, msg_ptr_len, 0);
    CHK_ERR(ret, "Send failure");
  }

  free(resp_msg_ptr);
}
