/******************************************************************************
#  Copyright (c) 2015, 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
/*!
  @file
  qcril_qmi_radio_config_socket.h

  @brief
  APIs for radio_config socket.
  Handles traffic to and fro the socket.
*/

#ifndef QCRIL_QMI_RADIO_CONFIG_SOCKET_H
#define QCRIL_QMI_RADIO_CONFIG_SOCKET_H

#ifdef __cplusplus
extern "C" {
#endif

#include "qcrili.h"
#include "telephony/ril.h"
#include "radio_config_interface.pb.h"

#define QCRIL_QMI_RADIO_CONFIG_SOCKET_MAX_BUF_SIZE (1024*8)
#define QCRIL_QMI_RADIO_CONFIG_SOCKET_MAX_THREAD_NAME_SIZE 50

void qcril_qmi_radio_config_socket_init();

void qcril_qmi_radio_config_socket_send
(
  RIL_Token token,
  com_qualcomm_qti_radioconfiginterface_MessageType type,
  com_qualcomm_qti_radioconfiginterface_MessageId message_id,
  bool has_error,
  com_qualcomm_qti_radioconfiginterface_Error error,
  const void* msg, int msg_len
);


void qcril_qmi_radio_config_socket_send_error_response
(
  RIL_Token token,
  com_qualcomm_qti_radioconfiginterface_MessageId message_id
);

#ifdef __cplusplus
}
#endif

#endif /* QCRIL_QMI_RADIO_CONFIG_SOCKET_H */
