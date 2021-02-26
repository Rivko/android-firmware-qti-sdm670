/******************************************************************************
#  Copyright (c) 2016-2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
/*!
  @file
  qcril_qmi_lte_direct_disc_packing.h

  @brief
  Handles lte direct discovery message packing/unpacking related functions.

*/


#ifndef QCRIL_QMI_LTE_DIRECT_DISC_PACKING_H
#define QCRIL_QMI_LTE_DIRECT_DISC_PACKING_H

/*===========================================================================
                           INCLUDE FILES
===========================================================================*/

#include "qcrili.h"
#include "qcril_qmi_lte_direct_disc_misc.h"

/*===========================================================================
                        FUNCTIONS
===========================================================================*/
size_t qcril_qmi_lte_direct_disc_pack_msg_tag(
    const void *msg,
    int msg_len,
    uint32_t token,
    LteDirectDiscovery_MsgType type,
    LteDirectDiscovery_MsgId message_id,
    LteDirectDiscovery_Error error,
    uint8_t *buf, size_t buf_size);

LteDirectDiscovery_MsgHeader* qcril_qmi_lte_direct_disc_unpack_msg_tag(
    const uint8_t *buf,
    size_t buf_size);

void qcril_qmi_lte_direct_disc_parse_packed_msg(
    LteDirectDiscovery_MsgType msg_type,
    LteDirectDiscovery_MsgId msg_id,
    const pb_callback_t packed_msg, size_t msg_size,
    void** unpacked_msg, size_t*unpacked_msg_size_ptr,
    qcril_evt_e_type* event_ptr);


#endif /* QCRIL_QMI_LTE_DIRECT_DISC_PACKING_H */
