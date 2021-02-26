/*!
  @file
  qcril_qmi_oem_msg_meta.h

  @brief

*/

/*===========================================================================

  Copyright (c) 2015 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
===========================================================================*/

#ifndef QCRIL_QMI_OEM_MSG_META_H
#define QCRIL_QMI_OEM_MSG_META_H

#include "qtiTelephonyService.pb.h"
#include "qcrili.h"
#include "qcril_qmi_oem_misc.h"
#include "qcril_qmi_npb_utils.h"

size_t qcril_qmi_oem_proto_get_msg_size(QtiTelephonyService_RILOEMMessageId msg_id, QtiTelephonyService_RILOEMMessageType msg_type);
const pb_field_t* qcril_qmi_oem_proto_get_msg_pb_fields(QtiTelephonyService_RILOEMMessageId msg_id, QtiTelephonyService_RILOEMMessageType msg_type);
const char* qcril_qmi_oem_proto_get_msg_log_str(QtiTelephonyService_RILOEMMessageId msg_id, QtiTelephonyService_RILOEMMessageType msg_type);
qcril_evt_e_type qcril_qmi_oem_proto_get_msg_event(QtiTelephonyService_RILOEMMessageId msg_id, QtiTelephonyService_RILOEMMessageType msg_type);
boolean qcril_qmi_oem_proto_is_msg_valid(QtiTelephonyService_RILOEMMessageId msg_id, QtiTelephonyService_RILOEMMessageType msg_type);

#endif /* QCRIL_QMI_OEM_MSG_META_H */
