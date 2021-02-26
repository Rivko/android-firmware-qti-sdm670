/******************************************************************************
#  Copyright (c) 2012, 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
/*!
  @file
  qcril_qmi_ims_packing.h

  @brief

*/


#ifndef QCRIL_QMI_IMS_PACKING_H
#define QCRIL_QMI_IMS_PACKING_H

#include "qcrili.h"
#include <modules/ims/imsIF.pb.h>

#ifdef __cplusplus
extern "C" {
#endif

size_t qcril_qmi_ims_pack_msg_tag(uint32_t token, ims_MsgType type, ims_MsgId message_id, ims_Error error, uint8_t *buf, size_t buf_size);
ims_MsgTag* qcril_qmi_ims_unpack_msg_tag(const uint8_t *buf);

size_t qcril_qmi_ims_pack_msg(const void *msg, ims_MsgType type, ims_MsgId message_id, uint8_t *buf, size_t max_buf_size);
void qcril_qmi_ims_parse_packed_msg(ims_MsgType type, ims_MsgId message_id, const uint8_t *packed_msg, size_t packed_msg_size,
                                     void** unpacked_msg, size_t*unpacked_msg_size_ptr, qcril_evt_e_type* event_ptr);
#ifdef __cplusplus
}
#endif

#endif /* QCRIL_QMI_IMS_PACKING_H */
