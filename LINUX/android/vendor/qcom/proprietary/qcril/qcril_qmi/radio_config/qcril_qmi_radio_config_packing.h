/*!
  @file
  qcril_qmi_radio_config_socket.h

  @brief
  APIs for radio_config socket.
  Handles traffic to and fro that socket.
*/
/*===========================================================================

Copyright (c) 2015 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.

===========================================================================*/

#ifndef QCRIL_QMI_RADIO_CONFIG_PACKING_H
#define QCRIL_QMI_RADIO_CONFIG_PACKING_H

#include "radio_config_interface.pb.h"
#include "qcrili.h"

size_t qcril_qmi_radio_config_pack_msg_and_tag
(
   const void *msg,
   int msg_len,
   uint32_t token,
   com_qualcomm_qti_radioconfiginterface_MessageType type,
   com_qualcomm_qti_radioconfiginterface_MessageId message_id,
   bool has_error,
   com_qualcomm_qti_radioconfiginterface_Error error,
   uint8_t *buf,
   size_t buf_size
);

com_qualcomm_qti_radioconfiginterface_MessageTag* qcril_qmi_radio_config_unpack_msg_tag
(
	const uint8_t* buf,
	size_t buf_size
);

void qcril_qmi_radio_config_parse_packed_msg
(
	com_qualcomm_qti_radioconfiginterface_MessageType msg_type,
	com_qualcomm_qti_radioconfiginterface_MessageId msg_id,
	const pb_bytes_array_t* packed_msg,
	void** unpacked_msg,
	size_t* unpacked_msg_size_ptr,
	qcril_evt_e_type* event_ptr
);

com_qualcomm_qti_radioconfiginterface_LtedConfig* qcril_qmi_radio_config_unpack_lted_config
(
 const uint8_t* buf,
 size_t buf_size
);

pb_bytes_array_t* qcril_qmi_radio_config_pack_lted_config
(
 com_qualcomm_qti_radioconfiginterface_LtedConfig* msg
);

com_qualcomm_qti_radioconfiginterface_LtedCategory* qcril_qmi_radio_config_unpack_lted_category
(
 const uint8_t* buf,
 size_t buf_size
);

pb_bytes_array_t* qcril_qmi_radio_config_pack_lted_category
(
 com_qualcomm_qti_radioconfiginterface_LtedCategory* msg
);

#endif /* QCRIL_QMI_RADIO_CONFIG_PACKING_H */
