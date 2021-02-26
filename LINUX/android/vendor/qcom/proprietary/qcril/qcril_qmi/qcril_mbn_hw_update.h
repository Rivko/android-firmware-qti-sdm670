/******************************************************************************

  @file    qcril_mbn_reader.h
  @brief   Provides interface to read mbn file

  DESCRIPTION

  ---------------------------------------------------------------------------

  Copyright (c) 2015 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.

  ---------------------------------------------------------------------------
******************************************************************************/

#ifndef QCRIL_MBN_HW_UPDATE_H
#define QCRIL_MBN_HW_UPDATE_H

#include "qcrili.h"

typedef struct
{
    int modem_max_sub;
    int apss_max_sub;
} qcril_mbn_hw_init_req;

typedef void (*qcril_mbn_call_back_on_drop_type)();
/*===========================================================================

  FUNCTION qcril_mbn_hw_update_init

===========================================================================*/
/*!
    @brief
    Hardware update initilization

    @return
    E_SUCCESS or E_FAILURE
*/
/*=========================================================================*/
IxErrnoType qcril_mbn_hw_update_init
(
    int modem_max_sub,
    int apss_max_sub,
    int is_modem_state_valid,
    boolean is_on_request
);

/*===========================================================================

  FUNCTION qcril_mbn_hw_update_init_hdlr

===========================================================================*/
/*!
    @brief
    Hardware update init handler

    @return
    E_SUCCESS or E_FAILURE
*/
/*=========================================================================*/
void qcril_mbn_hw_update_init_hdlr
(
    const qcril_request_params_type *const params_ptr,
    qcril_request_return_type       *const ret_ptr
);

/*===========================================================================

  FUNCTION qcril_mbn_hw_update_disabled_init_hdlr

===========================================================================*/
/*!
    @brief
    Hardware update disabled init handler

    @return
    E_SUCCESS or E_FAILURE
*/
/*=========================================================================*/
void qcril_mbn_hw_update_disabled_init_hdlr
(
    const qcril_request_params_type *const params_ptr,
    qcril_request_return_type       *const ret_ptr
);

/*===========================================================================

  FUNCTION qcril_mbn_hw_load_to_db

===========================================================================*/
/*!
    @brief
    Load hw mbn to db

    @return
    E_SUCCESS or E_FAILURE
*/
/*=========================================================================*/
IxErrnoType qcril_mbn_hw_load_to_db
(
    void
);

/*===========================================================================

  FUNCTION qcril_mbn_hw_send_deactivate_config_resp

===========================================================================*/
/*!
    @brief
    Send deactivate config response

    @return
    E_SUCCESS or E_FAILURE
*/
/*=========================================================================*/
IxErrnoType qcril_mbn_hw_send_deactivate_config_resp
(
    RIL_Errno result
);

/*===========================================================================

  FUNCTION qcril_mbn_hw_send_delete_config_resp

===========================================================================*/
/*!
    @brief
    Send delete config resp

    @return
    E_SUCCESS or E_FAILURE
*/
/*=========================================================================*/
IxErrnoType qcril_mbn_hw_send_delete_config_resp
(
    RIL_Errno result
);

/*===========================================================================

  FUNCTION qcril_mbn_hw_send_load_config_resp

===========================================================================*/
/*!
    @brief
    Send load_config response

    @return
    E_SUCCESS or E_FAILURE
*/
/*=========================================================================*/
IxErrnoType qcril_mbn_hw_send_load_config_resp
(
    uint32_t  error
);

/*===========================================================================

  FUNCTION qcril_mbn_hw_send_select_config_resp

===========================================================================*/
/*!
    @brief
    Send select_config response

    @return
    None
*/
/*=========================================================================*/
void qcril_mbn_hw_send_select_config_resp
(
    RIL_Errno result
);

/*===========================================================================

  FUNCTION qcril_mbn_hw_send_activate_config_resp

===========================================================================*/
/*!
    @brief
    Send activate_config response

    @return
    E_SUCCESS or E_FAILURE
*/
/*=========================================================================*/
IxErrnoType qcril_mbn_hw_send_activate_config_resp
(
    RIL_Errno result
);

/*===========================================================================

  FUNCTION qcril_mbn_hw_send_get_selected_resp

===========================================================================*/
/*!
    @brief
    Send get_selected response

    @return
    E_SUCCESS or E_FAILURE
*/
/*=========================================================================*/
IxErrnoType qcril_mbn_hw_send_get_selected_resp
(
    RIL_Errno result,
    uint8_t  *config_id,
    uint32_t  config_id_len
);

/*=========================================================================

  FUNCTION:  qcril_mbn_hw_activate_config_hndlr

===========================================================================*/
/*!
    @brief
    Handle QCRIL_EVT_QMI_RIL_PDC_MBN_HW_ACTIVATE_CONFIG

    @return
    None.
*/
/*=========================================================================*/
void qcril_mbn_hw_activate_config_hndlr
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type       *const ret_ptr
);

/*=========================================================================

  FUNCTION:  qcril_mbn_hw_select_config_hndlr

===========================================================================*/
/*!
    @brief
    Handle QCRIL_EVT_QMI_RIL_PDC_MBN_HW_SELECT_CONFIG

    @return
    None.
*/
/*=========================================================================*/
void qcril_mbn_hw_select_config_hndlr
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type       *const ret_ptr
);

/*=========================================================================

  FUNCTION:  qcril_mbn_hw_load_config_hndlr

===========================================================================*/
/*!
    @brief
    Handle QCRIL_EVT_QMI_RIL_PDC_MBN_HW_LOAD_CONFIG

    @return
    None.
*/
/*=========================================================================*/
void qcril_mbn_hw_load_config_hndlr
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type       *const ret_ptr
);


/*=========================================================================

  FUNCTION:  qcril_mbn_hw_delete_config_hndlr

===========================================================================*/
/*!
    @brief
    Handle QCRIL_EVT_QMI_RIL_PDC_MBN_HW_DELETE_CONFIG

    @return
    None.
*/
/*=========================================================================*/
void qcril_mbn_hw_delete_config_hndlr
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type       *const ret_ptr
);


/*=========================================================================

  FUNCTION:  qcril_mbn_hw_deactivate_config_hndlr

===========================================================================*/
/*!
    @brief
    Handle QCRIL_EVT_QMI_RIL_PDC_MBN_HW_DEACTIVATE_CONFIG

    @return
    None.
*/
/*=========================================================================*/
void qcril_mbn_hw_deactivate_config_hndlr
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type       *const ret_ptr
);

/*===========================================================================

  FUNCTION qcril_mbn_hw_query_selected_config_hndlr

===========================================================================*/
/*!
    @brief
    Query selected confuration handler

    @return
    E_SUCCESS or E_FAILURE
*/
/*=========================================================================*/
void qcril_mbn_hw_query_selected_config_hndlr
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type       *const ret_ptr
);

/*===========================================================================

  FUNCTION qcril_mbn_hw_is_completed

===========================================================================*/
/*!
    @brief
    Check if mbn hw is completed

    @return
    E_SUCCESS or E_FAILURE
*/
/*=========================================================================*/
boolean qcril_mbn_hw_is_completed
(
    void
);

/*===========================================================================

  FUNCTION qcril_mbn_hw_inform_wait_drop

===========================================================================*/
/*!
    @brief
    Inform if process is dropped

    @return
    E_SUCCESS or E_FAILURE
*/
/*=========================================================================*/
void qcril_mbn_hw_inform_wait_drop
(
    qcril_mbn_call_back_on_drop_type call_back_on_drop
);

/*===========================================================================

  FUNCTION qcril_qmi_hw_mbn_reboot_cleanup

===========================================================================*/
/*!
    @brief
    Clean up hw mbn state

    @return
    None
*/
/*=========================================================================*/
void qcril_qmi_hw_mbn_reboot_cleanup (void);

/*===========================================================================

  FUNCTION qcril_qmi_hw_mbn_db_loaded

===========================================================================*/
/*!
    @brief
    Check if hw db is loaded or not

    @return
    1 if db is loaded, 0 otherwise
*/
/*=========================================================================*/
int qcril_qmi_hw_mbn_loaded_to_db (void);

void qcril_qmi_mbn_hw_init
(
  void
);
#endif /* QCRIL_MBN_HW_UPDATE_H */
