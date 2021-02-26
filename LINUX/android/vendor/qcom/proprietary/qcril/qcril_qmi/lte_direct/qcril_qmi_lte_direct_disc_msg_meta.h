/*!
  @file
  qcril_qmi_lte_direct_disc_msg_meta.h

  @brief
  Handles lte direct discovery message meta info related functions.

*/

/*===========================================================================

 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.

===========================================================================*/

#ifndef QCRIL_QMI_LTE_DIRECT_DISC_MSG_META_H
#define QCRIL_QMI_LTE_DIRECT_DISC_MSG_META_H

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

#endif /* QCRIL_QMI_LTE_DIRECT_DISC_MSG_META_H */
