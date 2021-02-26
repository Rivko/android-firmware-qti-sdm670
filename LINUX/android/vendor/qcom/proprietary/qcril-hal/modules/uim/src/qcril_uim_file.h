#ifndef QCRIL_UIM_FILE_H
#define QCRIL_UIM_FILE_H
/******************************************************************************
#  Copyright (c) 2017-2018 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#include "modules/uim/qcril_uim.h"


/*=========================================================================

  QCRIL_UIM_CARD_APP_TYPE

===========================================================================*/
typedef struct
{
  char              * aid;
  qmi_uim_app_type    app_type;
} qcril_uim_card_app_type;


/*=========================================================================

  FUNCTION:  qcril_uim_read_binary_resp

===========================================================================*/
/*!
    @brief
    Process the response for read binary.

    @return
    None
*/
/*=========================================================================*/
void qcril_uim_read_binary_resp
(
  const qcril_uim_callback_params_type * const params_ptr
);


/*=========================================================================

  FUNCTION:  qcril_uim_update_binary_resp

===========================================================================*/
/*!
    @brief
    Process the response for write transparent.

    @return
    None
*/
/*=========================================================================*/
void qcril_uim_update_binary_resp
(
  const qcril_uim_callback_params_type * const params_ptr
);


/*=========================================================================

  FUNCTION:  qcril_uim_read_record_resp

===========================================================================*/
/*!
    @brief
    Process the response for read record.

    @return
    None
*/
/*=========================================================================*/
void qcril_uim_read_record_resp
(
  const qcril_uim_callback_params_type * const params_ptr
);

/*=========================================================================

  FUNCTION:  qcril_uim_update_record_resp

===========================================================================*/
/*!
    @brief
    Process the response for write record.

    @return
    None
*/
/*=========================================================================*/
void qcril_uim_update_record_resp
(
  const qcril_uim_callback_params_type * const params_ptr
);


/*=========================================================================

  FUNCTION:  qcril_uim_get_response_resp

===========================================================================*/
/*!
    @brief
    Process the response for get file attributes.

    @return
    None
*/
/*=========================================================================*/
void qcril_uim_get_response_resp
(
  const qcril_uim_callback_params_type * const params_ptr
);


/*=========================================================================

  FUNCTION:  qcril_uim_get_imsi_resp

===========================================================================*/
/*!
    @brief
    Process the response for RIL_REQUEST_GET_IMSI.

    @return
    None
*/
/*=========================================================================*/
void qcril_uim_get_imsi_resp
(
  const qcril_uim_callback_params_type * const params_ptr
);


/*=========================================================================

  FUNCTION:  qcril_uim_get_fdn_status_resp

===========================================================================*/
/*!
    @brief
    Process the response for get FDN status.

    @return
    None
*/
/*=========================================================================*/
void qcril_uim_get_fdn_status_resp
(
  const qcril_uim_callback_params_type * const params_ptr
);


/*=========================================================================

  FUNCTION:  qcril_uim_set_fdn_status_resp

===========================================================================*/
/*!
    @brief
    Process the response for set FDN status.

    @return
    None
*/
/*=========================================================================*/
void qcril_uim_set_fdn_status_resp
(
  const qcril_uim_callback_params_type * const params_ptr
);


#if defined(RIL_REQUEST_SIM_OPEN_CHANNEL) || defined(RIL_REQUEST_SIM_CLOSE_CHANNEL)
/*=========================================================================

  FUNCTION:  qcril_uim_logical_channel_resp

===========================================================================*/
/*!
    @brief
    Process the response for logical channel command.

    @return
    None
*/
/*=========================================================================*/
void qcril_uim_logical_channel_resp
(
  const qcril_uim_callback_params_type * const params_ptr
);
#endif /* RIL_REQUEST_SIM_OPEN_CHANNEL || RIL_REQUEST_SIM_CLOSE_CHANNEL */


#if defined(RIL_REQUEST_SIM_TRANSMIT_APDU_BASIC) || defined(RIL_REQUEST_SIM_TRANSMIT_APDU_CHANNEL)
/*=========================================================================

  FUNCTION:  qcril_uim_reselect_resp

===========================================================================*/
/*!
    @brief
    Process the response for reselect. Note that this request comes via the
    send apdu API, hence the response uses that data type.

    @return
    None
*/
/*=========================================================================*/
void qcril_uim_reselect_resp
(
  const qcril_uim_callback_params_type * const params_ptr
);


/*=========================================================================

  FUNCTION:  qcril_uim_send_apdu_resp

===========================================================================*/
/*!
    @brief
    Process the response for logical channel command.

    @return
    None
*/
/*=========================================================================*/
void qcril_uim_send_apdu_resp
(
  const qcril_uim_callback_params_type * const params_ptr
);


/*===========================================================================

  FUNCTION:  qcril_uim_process_send_apdu_ind

===========================================================================*/
/*!
    @brief
    Function for processing send APDU indication. Based on the data received
    in the send APDU response, this routine is responsible for concatenating
    all the chunks of the APDU indication & preparing & sending one long APDU
    to the client.

    @return
    None.
*/
/*=========================================================================*/
void qcril_uim_process_send_apdu_ind
(
  const qcril_uim_indication_params_type  * ind_param_ptr,
  qcril_request_return_type               * const ret_ptr /*!< Output parameter */
);


/*=========================================================================

  FUNCTION:  qcril_uim_set_apdu_behavior_resp

===========================================================================*/
/*!
    @brief
    Processes the response for set APDU behavior command.

    @return
    None
*/
/*=========================================================================*/
void qcril_uim_set_apdu_behavior_resp
(
  const qcril_uim_callback_params_type * const params_ptr
);
#endif /* RIL_REQUEST_SIM_TRANSMIT_APDU_BASIC || RIL_REQUEST_SIM_TRANSMIT_APDU_CHANNEL */

/*=========================================================================

  FUNCTION:  qcril_uim_sim_authenticate_resp

===========================================================================*/
/*!
    @brief
    Process the response for SIM authenticate command.

    @return
    None
*/
/*=========================================================================*/
void qcril_uim_sim_authenticate_resp
(
  const qcril_uim_callback_params_type * const params_ptr
);


/*=========================================================================

  FUNCTION:  qcril_uim_send_status_resp

===========================================================================*/
/*!
    @brief
    Process the response for STATUS command.

    @return
    None
*/
/*=========================================================================*/
void qcril_uim_send_status_resp
(
  const qcril_uim_callback_params_type * const params_ptr
);

/*=========================================================================

  FUNCTION:  qcril_uim_request_get_mcc_mnc

===========================================================================*/
/*!
    @brief
    Process the request to get MCC and MNC

    @return
    None
*/
/*=========================================================================*/
void qcril_uim_request_get_mcc_mnc
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type       *const ret_ptr /*!< Output parameter */
);


/*=========================================================================

  FUNCTION:  qcril_uim_get_mcc_mnc_resp

===========================================================================*/
/*!
    @brief
    Processes the response for QCRIL_EVT_INTERNAL_UIM_GET_MCC_MNC

    @return
    None
*/
/*=========================================================================*/
void qcril_uim_get_mcc_mnc_resp
(
  const qcril_uim_callback_params_type * const params_ptr
);


/*===========================================================================

  FUNCTION:  qcril_uim_request_get_fdn_status

===========================================================================*/
/*!
    @brief
    Handles QCRIL_EVT_INTERNAL_MMGSDI_GET_FDN_STATUS request from QCRIL.
    This is due to handling of RIL_REQUEST_QUERY_FACILITY_LOCK with facility
    string "FD" from the framework.

    @return
    None
*/
/*=========================================================================*/
void qcril_uim_request_get_fdn_status
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type       *const ret_ptr /*!< Output parameter */
);


/*===========================================================================

  FUNCTION:  qcril_uim_request_set_fdn_status

===========================================================================*/
/*!
    @brief
    Handles QCRIL_EVT_INTERNAL_MMGSDI_SET_FDN_STATUS request from QCRIL.
    This is due to handling of RIL_REQUEST_SET_FACILITY_LOCK with facility
    string "FD" from the framework.

    @return
    None
*/
/*=========================================================================*/
void qcril_uim_request_set_fdn_status
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type       *const ret_ptr /*!< Output parameter */
);


/*===========================================================================

  FUNCTION:  qcril_uim_process_internal_verify_pin_command_callback

===========================================================================*/
/*!
    @brief
    Handler for QCRIL_EVT_INTERNAL_UIM_VERIFY_PIN_COMMAND_CALLBACK.

    @return
    None
*/
/*=========================================================================*/
void qcril_uim_process_internal_verify_pin_command_callback
(
  qcril_uim_callback_params_type             * params_ptr
);


/*===========================================================================

  FUNCTION:  qcril_uim_cleanup_long_apdu_info

===========================================================================*/
/*!
    @brief
    Frees if any memory is allocated in global APDU info structure. It also
    sends an error response in case the original request pointer is still
    pending.

    @return
    None.
*/
/*=========================================================================*/
void qcril_uim_cleanup_long_apdu_info
(
  void
);


/*===========================================================================

  FUNCTION:  qcril_uim_cleanup_select_response_info

===========================================================================*/
/*!
    @brief
    Cleans the global select response info structure & frees any memory
    allocated for the raw response data.

    @return
    None.
*/
/*=========================================================================*/
void qcril_uim_cleanup_select_response_info
(
  void
);

/*===========================================================================

  FUNCTION:  qcril_uim_cleanup_ara_m_delay_cache_info

===========================================================================*/
/*!
    @brief
    Cleans the global ARA-M APP open cache and sends error response.

    @return
    None.
*/
/*=========================================================================*/
void qcril_uim_cleanup_ara_m_delay_cache_info
(
  void
);

/*===========================================================================

  FUNCTION:  qcril_uim_direct_get_imsi

===========================================================================*/
/*!
    @brief
    Returns IMSI of requested aid of a slot

    @return
    None

    Note: imsi_ptr need to delete by client
*/
/*=========================================================================*/
RIL_Errno qcril_uim_direct_get_imsi
(
  uint8            slot,
  const char     * aid_ptr,
  char          ** imsi_ptr
);

/*=========================================================================

  FUNCTION:  qmi_uim_internal_pin2_callback

===========================================================================*/
/*!
    @brief
    Callback for the PIN2 verification before executing another
    function that accesses the files on the card (read or write).
    This function posts an event, so it can be processed in the
    QCRIL context.

    @return
    None
*/
/*=========================================================================*/
void qmi_uim_internal_pin2_callback
(
  qmi_uim_rsp_data_type        * rsp_data,
  void                         * user_data
);

/*===========================================================================

  FUNCTION:  qcril_uim_request_get_imsi

===========================================================================*/
/*!
    @brief
    Handler for RIL_REQUEST_GET_IMSI.

    @return
    None
*/
/*=========================================================================*/
void qcril_uim_request_get_imsi
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type       *const ret_ptr /*!< Output parameter */
);

/*=========================================================================

  FUNCTION:  qcril_uim_request_sim_io

===========================================================================*/
/*!
    @brief
    Handler for RIL_REQUEST_SIM_IO.

    @return
    None
*/
/*=========================================================================*/
void qcril_uim_request_sim_io
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type       *const ret_ptr /*!< Output parameter */
);

/*===========================================================================

  FUNCTION:  qcril_uim_request_send_apdu

===========================================================================*/
/*!
    @brief
    Handles these RIL requests:

    RIL_REQUEST_SIM_TRANSMIT_APDU_BASIC
    RIL_REQUEST_SIM_TRANSMIT_APDU_CHANNEL

    @return
    None
*/
/*=========================================================================*/
void qcril_uim_request_send_apdu
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type       *const ret_ptr /*!< Output parameter */
);

/*=========================================================================

  FUNCTION:  qcril_uim_request_set_apdu_behavior

===========================================================================*/
/*!
    @brief
    Handles QCRIL_EVT_HOOK_SLOT_SET_APDU_BEHAVIOR_REQ request from QCRIL.

    @return
    None
*/
/*=========================================================================*/
void qcril_uim_request_set_apdu_behavior
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type       *const ret_ptr /*!< Output parameter */
);

/*===========================================================================

  FUNCTION:  qcril_uim_request_isim_authenticate

===========================================================================*/
/*!
    @brief
    Handler for RIL_REQUEST_ISIM_AUTHENTICATION.

    @return
    None
*/
/*=========================================================================*/
void qcril_uim_request_isim_authenticate
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type       *const ret_ptr /*!< Output parameter */
);

/*===========================================================================

  FUNCTION:  qcril_uim_request_logical_channel

===========================================================================*/
/*!
    @brief
    Handles RIL_REQUEST_SIM_OPEN_CHANNEL or RIL_REQUEST_SIM_CLOSE_CHANNEL
    request from QCRIL.

    @return
    None
*/
/*=========================================================================*/
void qcril_uim_request_logical_channel
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type       *const ret_ptr /*!< Output parameter */
);

/*===========================================================================

  FUNCTION:  qcril_uim_request_sim_authenticate

===========================================================================*/
/*!
    @brief
    Handler for RIL_REQUEST_SIM_AUTHENTICATION.

    @return
    None
*/
/*=========================================================================*/
void qcril_uim_request_sim_authenticate
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type       *const ret_ptr /*!< Output parameter */
);

/*=========================================================================

  FUNCTION:  qcril_uim_request_get_terminal_capability

===========================================================================*/
/*!
    @brief
    Handles QCRIL_EVT_HOOK_SLOT_GET_TERMINAL_CAPABILITY_LPA_TLV_REQ request
    from QCRIL.

    @return
    None
*/
/*=========================================================================*/
void qcril_uim_request_get_terminal_capability
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type       *const ret_ptr /*!< Output parameter */
);

/*=========================================================================

  FUNCTION:  qcril_uim_request_set_terminal_capability

===========================================================================*/
/*!
    @brief
    Handles QCRIL_EVT_HOOK_SLOT_SET_TERMINAL_CAPABILITY_LPA_TLV_REQ request
    from QCRIL.

    @return
    None
*/
/*=========================================================================*/
void qcril_uim_request_set_terminal_capability
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type       *const ret_ptr /*!< Output parameter */
);

/*=========================================================================

  FUNCTION:  qcril_uim_get_terminal_capability_resp

===========================================================================*/
/*!
    @brief
    Processes the response for get terminal capability command.

    @return
    None
*/
/*=========================================================================*/
void qcril_uim_get_terminal_capability_resp
(
  const qcril_uim_callback_params_type * const params_ptr
);

/*===========================================================================*/
/*!
    @brief
    Processes the response for set terminal capability command.

    @return
    None
*/
/*=========================================================================*/
void qcril_uim_set_terminal_capability_resp
(
  const qcril_uim_callback_params_type * const params_ptr
);

#endif /* QCRIL_UIM_FILE_H */

