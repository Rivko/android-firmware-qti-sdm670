/*===========================================================================

   Copyright (c) 2012, 2017 Qualcomm Technologies, Inc.
   All Rights Reserved.
   Confidential and Proprietary - Qualcomm Technologies, Inc.

===========================================================================*/
/*!
  @file
  qcril_qmi_ims_radio_service.h

  @brief

*/

#ifndef QCRIL_QMI_IMS_RADIO_AGENT_H
#define QCRIL_QMI_IMS_RADIO_AGENT_H

#include "qcrili.h"
#include <modules/ims/imsIF.pb.h>

#ifdef  __cplusplus
extern "C" {
#endif

#include "telephony/ril.h"

boolean isImsRadioConnected();
void imsRadioInit();
errno_enum_type imsRadioSendMessage(RIL_Token token, ims_MsgType type,
        ims_MsgId message_id, ims_Error error, const void* msg, int msg_len);
void imsRadioSendMessageToFlowControl(RIL_Token token, ims_MsgType type,
        ims_MsgId message_id, ims_Error error, const void* msg, int msg_len);
void imsRadioSendEmptyUnsolResponse(ims_MsgId msg_id);

#ifdef  __cplusplus
}
#endif
#endif  // QCRIL_QMI_IMS_RADIO_AGENT_H
