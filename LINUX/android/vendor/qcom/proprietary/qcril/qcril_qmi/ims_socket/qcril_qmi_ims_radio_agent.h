/*!
  @file
  qcril_qmi_ims_radio_service.h

  @brief

*/

/*===========================================================================

   Copyright (c) 2017 Qualcomm Technologies, Inc.
   All Rights Reserved.
   Confidential and Proprietary - Qualcomm Technologies, Inc.

===========================================================================*/

#ifndef QCRIL_QMI_IMS_RADIO_AGENT_H
#define QCRIL_QMI_IMS_RADIO_AGENT_H

#ifdef  __cplusplus
extern "C" {
#endif

#include "imsIF.pb.h"
#include "ril.h"
#include "qcrili.h"

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
