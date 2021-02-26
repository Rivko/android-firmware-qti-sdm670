/******************************************************************************
#  Copyright (c) 2008-2009, 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
/*!
  @file
  qcril_qmi_ims.h

  @brief

*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.


when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/06/10   sk      Initial Changes

===========================================================================*/

#ifndef QCRIL_QMI_IMS_H
#define QCRIL_QMI_IMS_H

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#include "comdef.h"
#include "qmi_client.h"
#include "qmi_ims_vt_v01.h"
#include "ip_multimedia_subsystem_presence_v01.h"
/*===========================================================================

                        DEFINITIONS AND TYPES

===========================================================================*/

#define QCRIL_QMI_VOICE_RIL_PI_ALLOWED 0
#define QCRIL_QMI_VOICE_RIL_PI_RESTRICTED 1
#define QCRIL_QMI_VOICE_RIL_PI_UNKNOWN 2
#define QCRIL_QMI_VOICE_RIL_PI_PAYPHONE 3

#define QCRIL_QMI_VOICE_INVALID_CONN_ID         0
#define QCRIL_QMI_VOICE_UNKNOWN_ACTIVE_CONN_ID  0xFF


/* CLIR Persistent System Property */
#define QCRIL_QMI_VOICE_CLIR                         "persist.vendor.radio.clir"

#define QCRIL_QMI_VOICE_SS_TA_UNKNOWN       129 /* 0x80|CM_TON_UNKNOWN      |CM_NPI_ISDN */
#define QCRIL_QMI_VOICE_SS_TA_INTERNATIONAL 145 /* 0x80|CM_TON_INTERNATIONAL|CM_NPI_ISDN */
#define QCRIl_QMI_VOICE_SS_TA_INTER_PREFIX  '+' /* ETSI international call dial prefix */

// backport from cm.h -- start
#define SYS_MAX_PRIMARY_PDP_CONTEXTS          3
#define SYS_MAX_SEC_PDP_CONTEXT_PER_PRIMARY   2

#define SYS_MAX_PDP_CONTEXTS                                                 \
  ( SYS_MAX_PRIMARY_PDP_CONTEXTS +                                           \
    ( SYS_MAX_PRIMARY_PDP_CONTEXTS * SYS_MAX_SEC_PDP_CONTEXT_PER_PRIMARY ))

#define CM_CALL_ID_MAX              ( 6 + SYS_MAX_PDP_CONTEXTS + \
                                      CM_MAX_EPS_BEARERS_CONTEXTS )

#define SYS_MAX_EPS_BEARERS_CONTEXTS          8

#define CM_MAX_EPS_BEARERS_CONTEXTS ( SYS_MAX_EPS_BEARERS_CONTEXTS - \
                                      SYS_MAX_PRIMARY_PDP_CONTEXTS )

typedef struct
{
  uint8     last_incom_connid;
}qcril_qmi_ims_info_type;

#ifdef __cplusplus
extern "C" {
#endif

void qcril_qmi_ims_vt_command_cb
(
  qmi_client_type              user_handle,
  unsigned int                 msg_id,
  void                        *resp_c_struct,
  unsigned int                 resp_c_struct_len,
  void                        *resp_cb_data,
  qmi_client_error_type        transp_err
);

void qcril_qmi_ims_vt_unsol_ind_cb
(
  qmi_client_type                user_handle,
  unsigned int                   msg_id,
  void                          *ind_buf,
  unsigned int                   ind_buf_len,
  void                          *ind_cb_data
);

void qcril_qmi_ims_presence_command_cb
(
  qmi_client_type              user_handle,
  unsigned int                 msg_id,
  void                        *resp_c_struct,
  unsigned int                 resp_c_struct_len,
  void                        *resp_cb_data,
  qmi_client_error_type        transp_err
);

void qcril_qmi_ims_presence_unsol_ind_cb
(
  qmi_client_type                user_handle,
  unsigned int                   msg_id,
  void                          *ind_buf,
  unsigned int                   ind_buf_len,
  void                          *ind_cb_data
);

void qcril_qmi_ims_init
(
  void
);

#ifdef QMI_RIL_UTF
void qcril_qmi_hal_ims_module_cleanup();
#endif

#ifdef __cplusplus
}
#endif
#endif /* QCRIL_QMI_VOICE_H */

