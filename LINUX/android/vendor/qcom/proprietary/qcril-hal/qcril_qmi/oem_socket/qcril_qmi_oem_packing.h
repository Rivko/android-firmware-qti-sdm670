/******************************************************************************
#  Copyright (c) 2015, 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
/*!
  @file
  qcril_qmi_oem_packing.h

  @brief
   APIs to pack the qcril oem messages
*/


#ifndef QCRIL_QMI_OEM_PACKING_H
#define QCRIL_QMI_OEM_PACKING_H

#include "qtiTelephonyService.pb.h"
#include "qcrili.h"

size_t qcril_qmi_oem_proto_pack_msg_and_tag(const void *msg, uint32_t token,
                                                QtiTelephonyService_RILOEMMessageType type,
                                                QtiTelephonyService_RILOEMMessageId message_id,
                                                bool has_error, QtiTelephonyService_RILOEMError error,
                                                uint8_t *buf, size_t buf_size);

QtiTelephonyService_RILOEMMessage* qcril_qmi_oem_proto_unpack_msg_tag(const uint8_t *buf, size_t buf_size);

void qcril_qmi_oem_proto_parse_packed_msg(QtiTelephonyService_RILOEMMessageType msg_type,
                                              QtiTelephonyService_RILOEMMessageId msg_id,
                                              const pb_callback_t packed_msg, size_t msg_size,
                                              void** unpacked_msg, size_t*unpacked_msg_size_ptr,
                                              qcril_evt_e_type* event_ptr);

RIL_Errno qcril_qmi_oem_proto_map_oem_error_to_ril_error(QtiTelephonyService_RILOEMError oem_error);

#endif /* QCRIL_QMI_OEM_PACKING_H */
