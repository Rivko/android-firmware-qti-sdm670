/******************************************************************************

  @file    qcril_mbn_reader.h
  @brief   Provides interface to read mbn file

  DESCRIPTION

  ---------------------------------------------------------------------------

  Copyright (c) 2015 Qualcomm Technologies, Inc.  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

  ---------------------------------------------------------------------------
******************************************************************************/

#ifndef QCRIL_MBN_SW_UPDATE_H
#define QCRIL_MBN_SW_UPDATE_H

#include "qcrili.h"

typedef struct
{
    char *iccid;
    char  iccid_len;
} qcril_mbn_sw_init_req;

typedef enum {
    QCRIL_MBN_SW_MODEM_SWITCH_STATE_NONE           = 0,
    QCRIL_MBN_SW_MODEM_SWITCH_STATE_START          = 1,
    QCRIL_MBN_SW_MODEM_SWITCH_STATE_APPLY          = 2,
    QCRIL_MBN_SW_MODEM_SWITCH_STATE_COMPLETED      = 3
} qcril_mbn_sw_modem_switch_state;

/*===========================================================================

  FUNCTION qcril_mbn_sw_update_init

===========================================================================*/
/*!
    @brief
    Initilize software mbn update

    @return
    E_SUCCESS or E_FAILURE
*/
/*=========================================================================*/
IxErrnoType qcril_mbn_sw_update_init
(
    boolean is_ssr_or_bootup
);

/*===========================================================================

  FUNCTION qcril_mbn_sw_disable_modem_update_hndlr

===========================================================================*/
/*!
    @brief
    disable modem mbn auto selection feature

    @return
    None.
*/
/*=========================================================================*/
void qcril_mbn_sw_disable_modem_update_hndlr
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type       *const ret_ptr
);

/*===========================================================================

  FUNCTION qcril_mbn_sw_send_disable_modem_update_resp

===========================================================================*/
/*!
    @brief
    send response of disable modem update result, and trigger sw mbn update
    if the response is successful

    @return
    E_SUCCESS or E_FAILURE
*/
/*=========================================================================*/
IxErrnoType qcril_mbn_sw_send_disable_modem_update_resp
(
    RIL_Errno result,
    bool is_from_ind
);

/*===========================================================================

  FUNCTION qcril_mbn_sw_update_init_helper

===========================================================================*/
/*!
    @brief
    Verify and Initilize software mbn update

    @return
    None.
*/
/*=========================================================================*/
void qcril_mbn_sw_update_init_helper
(
    const qcril_request_params_type *const params_ptr,
    qcril_request_return_type       *const ret_ptr
);

/*===========================================================================

  FUNCTION qcril_mbn_sw_update_init_hdlr

===========================================================================*/
/*!
    @brief
    Software mbn update initialization handler

    @return
    None.
*/
/*=========================================================================*/
void qcril_mbn_sw_update_init_hdlr
(
    const qcril_request_params_type *const params_ptr,
    qcril_request_return_type       *const ret_ptr
);

/*===========================================================================

  FUNCTION qcril_mbn_sw_load_to_db

===========================================================================*/
/*!
    @brief
    Load sw mbn to db

    @return
    E_SUCCESS or E_FAILURE
*/
/*=========================================================================*/
IxErrnoType qcril_mbn_sw_load_to_db
(
    void
);

/*===========================================================================

  FUNCTION qcril_mbn_sw_send_deactivate_config_resp

===========================================================================*/
/*!
    @brief
    Send deactivaet_config response

    @return
    E_SUCCESS or E_FAILURE
*/
/*=========================================================================*/
IxErrnoType qcril_mbn_sw_send_deactivate_config_resp
(
    RIL_Errno result
);

/*===========================================================================

  FUNCTION qcril_mbn_sw_send_delete_config_resp

===========================================================================*/
/*!
    @brief
    Send delete_config response

    @return
    E_SUCCESS or E_FAILURE
*/
/*=========================================================================*/
IxErrnoType qcril_mbn_sw_send_delete_config_resp
(
    RIL_Errno result
);

/*===========================================================================

  FUNCTION qcril_mbn_sw_send_load_config_resp

===========================================================================*/
/*!
    @brief
    Send load_config response

    @return
    E_SUCCESS or E_FAILURE
*/
/*=========================================================================*/
IxErrnoType qcril_mbn_sw_send_load_config_resp
(
    uint32_t  error
);

/*===========================================================================

  FUNCTION qcril_mbn_sw_send_select_config_resp

===========================================================================*/
/*!
    @brief
    Retrieves meta data from mbn file

    @return
    None
*/
/*=========================================================================*/
void qcril_mbn_sw_send_select_config_resp
(
    RIL_Errno result
);

/*===========================================================================

  FUNCTION qcril_mbn_sw_send_activate_config_resp

===========================================================================*/
/*!
    @brief
    Send activate_config response

    @return
    TRUE: if there is more expected activation indication
    FALSE: if all expected activation indications are reported
*/
/*=========================================================================*/
boolean qcril_mbn_sw_send_activate_config_resp
(
    RIL_Errno result,
    int       from_ind
);

/*===========================================================================

  FUNCTION qcril_mbn_sw_send_check_pending_response

===========================================================================*/
/*!
    @brief
    Sends the info that if there is any pending configuration

    @return
    None
*/
/*=========================================================================*/
void qcril_mbn_sw_send_check_pending_response
(
    uint8_t pending_valid
);

/*===========================================================================

  FUNCTION qcril_mbn_sw_send_get_selected_resp

===========================================================================*/
/*!
    @brief
    Send get_selected response

    @return
    E_SUCCESS or E_FAILURE
*/
/*=========================================================================*/
IxErrnoType qcril_mbn_sw_send_get_selected_resp
(
    RIL_Errno result,
    uint8_t  *config_id,
    uint32_t  config_id_len
);

/*===========================================================================

  FUNCTION qcril_mbn_sw_send_cleanup_config_resp

===========================================================================*/
/*!
    @brief
    Send cleanup_config response

    @return
    E_SUCCESS or E_FAILURE
*/
/*=========================================================================*/
IxErrnoType qcril_mbn_sw_send_cleanup_config_resp
(
    RIL_Errno result
);

/*=========================================================================

  FUNCTION:  qcril_mbn_sw_activate_config_hndlr

===========================================================================*/
/*!
    @brief
    Handle QCRIL_EVT_QMI_RIL_PDC_MBN_SW_ACTIVATE_CONFIG

    @return
    None.
*/
/*=========================================================================*/
void qcril_mbn_sw_activate_config_hndlr
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type       *const ret_ptr
);

/*=========================================================================

  FUNCTION:  qcril_mbn_sw_select_config_hndlr

===========================================================================*/
/*!
    @brief
    Handle QCRIL_EVT_QMI_RIL_PDC_MBN_SW_SELECT_CONFIG

    @return
    None.
*/
/*=========================================================================*/
void qcril_mbn_sw_select_config_hndlr
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type       *const ret_ptr
);

/*=========================================================================

  FUNCTION:  qcril_mbn_sw_load_config_hndlr

===========================================================================*/
/*!
    @brief
    Handle QCRIL_EVT_QMI_RIL_PDC_MBN_SW_LOAD_CONFIG

    @return
    None.
*/
/*=========================================================================*/
void qcril_mbn_sw_load_config_hndlr
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type       *const ret_ptr
);


/*=========================================================================

  FUNCTION:  qcril_mbn_sw_delete_config_hndlr

===========================================================================*/
/*!
    @brief
    Handle QCRIL_EVT_QMI_RIL_PDC_MBN_SW_DELETE_CONFIG

    @return
    None.
*/
/*=========================================================================*/
void qcril_mbn_sw_delete_config_hndlr
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type       *const ret_ptr
);


/*=========================================================================

  FUNCTION:  qcril_mbn_sw_deactivate_config_hndlr

===========================================================================*/
/*!
    @brief
    Handle QCRIL_EVT_QMI_RIL_PDC_MBN_SW_DEACTIVATE_CONFIG

    @return
    None.
*/
/*=========================================================================*/
void qcril_mbn_sw_deactivate_config_hndlr
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type       *const ret_ptr
);

/*===========================================================================

  FUNCTION qcril_mbn_sw_query_selected_config_hndlr

===========================================================================*/
/*!
    @brief
    Query selected confuration

    @return
    E_SUCCESS or E_FAILURE
*/
/*=========================================================================*/
void qcril_mbn_sw_query_selected_config_hndlr
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type       *const ret_ptr
);

/*===========================================================================

  FUNCTION qcril_mbn_sw_is_completed

===========================================================================*/
/*!
    @brief
    Check if sw mbn is completed

    @return
    E_SUCCESS or E_FAILURE
*/
/*=========================================================================*/
boolean qcril_mbn_sw_is_completed
(
    void
);

/*===========================================================================

  FUNCTION qcril_mbn_sw_get_config_info_hndlr

===========================================================================*/
/*!
    @brief
    Handle get_config_info

    @return
    E_SUCCESS or E_FAILURE
*/
/*=========================================================================*/
void qcril_mbn_sw_get_config_info_hndlr
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type       *const ret_ptr
);

/*=========================================================================

  FUNCTION:  qcril_mbn_sw_cleanup_config_hndlr

===========================================================================*/
/*!
    @brief
    Handle QCRIL_EVT_REQUEST_MBN_SW_CLEANUP_CONFIG

    @return
    None.
*/
/*=========================================================================*/
void qcril_mbn_sw_cleanup_config_hndlr
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type       *const ret_ptr
);


/*=========================================================================

  FUNCTION:  qcril_mbn_sw_count_pending_configs_hndlr

===========================================================================*/
/*!
    @brief
    Handle QCRIL_EVT_REQUEST_MBN_SW_COUNT_PENDING_CONFIGS

    @return
    None.
*/
/*=========================================================================*/
void qcril_mbn_sw_count_pending_configs_hndlr
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type       *const ret_ptr
);


/*===========================================================================

  FUNCTION  qcril_mbn_sw_retrieve_selected_config

===========================================================================*/
/*!
    @brief
    Get config_info

    @return
    E_SUCCESS or E_FAILURE
*/
/*=========================================================================*/
IxErrnoType qcril_mbn_sw_retrieve_config_info
(
    uint8_t *config_id,
    int32_t  config_id_len
);

/*===========================================================================

  FUNCTION qcril_qmi_sw_mbn_reboot_cleanup

===========================================================================*/
/*!
    @brief
    Clean up sw mbn state

    @return
    None
*/
/*=========================================================================*/
void qcril_qmi_sw_mbn_reboot_cleanup (void);

/*===========================================================================

  FUNCTION qcril_qmi_sw_mbn_db_loaded

===========================================================================*/
/*!
    @brief
    Check if sw db is loaded or not

    @return
    1 if db is loaded, 0 otherwise
*/
/*=========================================================================*/
int qcril_qmi_sw_mbn_loaded_to_db (void);

/*===========================================================================

  FUNCTION qcril_qmi_sw_mbn_modem_switch

===========================================================================*/
/*!
    @brief
    Inform sw mbn update subsystem about modem switch

    @return
    None
*/
/*=========================================================================*/
void qcril_qmi_sw_mbn_modem_switch(qcril_mbn_sw_modem_switch_state state);

/*===========================================================================

  FUNCTION:  qcril_qmi_mbn_sw_init
===========================================================================*/
/*!
    @brief
    Initialize sw update subsystem.

    @return
    None.
*/
 /*=========================================================================*/

void qcril_qmi_mbn_sw_init
(
  void
);

#endif /* QCRIL_MBN_SW_UPDATE_H */
