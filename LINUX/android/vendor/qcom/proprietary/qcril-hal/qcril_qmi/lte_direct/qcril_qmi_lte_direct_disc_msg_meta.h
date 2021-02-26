/******************************************************************************
#  Copyright (c) 2016-2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
/*!
  @file
  qcril_qmi_lte_direct_disc_msg_meta.h

  @brief
  Handles lte direct discovery message meta info related functions.

*/


#ifndef QCRIL_QMI_LTE_DIRECT_DISC_MSG_META_H
#define QCRIL_QMI_LTE_DIRECT_DISC_MSG_META_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

/*===========================================================================
                           INCLUDE FILES
===========================================================================*/

#include "qcrili.h"
#include "qcril_qmi_lte_direct_disc_misc.h"
#include "qcril_qmi_npb_utils.h"

/*===========================================================================
                        FUNCTIONS
===========================================================================*/
size_t qcril_qmi_lte_direct_disc_get_msg_size(LteDirectDiscovery_MsgId msg_id, LteDirectDiscovery_MsgType msg_type);
const pb_field_t* qcril_qmi_lte_direct_disc_get_msg_pb_fields(LteDirectDiscovery_MsgId msg_id, LteDirectDiscovery_MsgType msg_type);
const char* qcril_qmi_lte_direct_disc_get_msg_log_str(LteDirectDiscovery_MsgId msg_id, LteDirectDiscovery_MsgType msg_type);
qcril_evt_e_type qcril_qmi_lte_direct_disc_get_msg_event(LteDirectDiscovery_MsgId msg_id, LteDirectDiscovery_MsgType msg_type);
boolean qcril_qmi_lte_direct_disc_is_msg_valid(LteDirectDiscovery_MsgId msg_id, LteDirectDiscovery_MsgType msg_type);
void qcril_qmi_lted_init_msg_meta();

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif /* QCRIL_QMI_LTE_DIRECT_DISC_MSG_META_H */
