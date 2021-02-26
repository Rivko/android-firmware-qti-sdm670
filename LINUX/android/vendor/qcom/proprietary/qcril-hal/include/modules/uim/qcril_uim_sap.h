#ifndef QCRIL_UIM_SAP_H
#define QCRIL_UIM_SAP_H
/*===========================================================================

  Copyright (c) 2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

===========================================================================*/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
#include "modules/uim/qcril_uim_srvc.h"
#include "modules/uim/UimCardStatusIndMsg.h"
#include "modules/uim/qcril_uim.h"

/*===========================================================================

TYPES and Definitions

===========================================================================*/

#define QCRIL_UIM_SAP_FREE_PTR(ptr)                                         \
  if (ptr != NULL)                                                          \
  {                                                                         \
    free(ptr);                                                              \
    ptr = NULL;                                                             \
  }                                                                         \

#define QCRIL_UIM_SAP_ALLOC_PTR_RETURN_IF_ERR(ptr,len)                      \
  ptr = malloc(len);                                                        \
  if (ptr == NULL)                                                          \
  {                                                                         \
    QCRIL_LOG_ERROR("%s","Error allocating request_ptr, cannot proceed");   \
    return NULL;                                                            \
  }                                                                         \

/* QCRIL SAP currently support only 2 max clients -
ATEL client & internal QCRIL  client for uim_remote_server service */
typedef enum
{
  QCRIL_UIM_SAP_CLIENT_ID_ATEL  = 0,
  QCRIL_UIM_SAP_CLIENT_ID_QCRIL = 1,
  QCRIL_UIM_SAP_MAX_CLIENTS
} qcril_uim_sap_clients_type;

typedef enum {
  QCRIL_UIM_SAP_REQUEST_REQ,
  QCRIL_UIM_SAP_CONNECTION_REQ
} qcril_uim_sap_request_type;

typedef enum {
  QCRIL_UIM_SAP_CONNECT_SUCCESS         = 0,
  QCRIL_UIM_SAP_CONNECT_FAILURE         = 1,
  QCRIL_UIM_SAP_MSG_SIZE_TOO_LARGE      = 2,
  QCRIL_UIM_SAP_MSG_SIZE_TOO_SMALL      = 3,
  QCRIL_UIM_SAP_CONNECT_OK_CALL_ONGOING = 4
} qcril_uim_sap_connect_response;

typedef enum {
  QCRIL_UIM_SAP_SUCCESS                  = 0,
  QCRIL_UIM_SAP_GENERIC_FAILURE          = 1,
  QCRIL_UIM_SAP_CARD_NOT_ACCESSSIBLE     = 2,
  QCRIL_UIM_SAP_CARD_ALREADY_POWERED_OFF = 3,
  QCRIL_UIM_SAP_CARD_REMOVED             = 4,
  QCRIL_UIM_SAP_CARD_ALREADY_POWERED_ON  = 5,
  QCRIL_UIM_SAP_DATA_NOT_AVAILABLE       = 6,
  QCRIL_UIM_SAP_NOT_SUPPORTED            = 7
} qcril_uim_sap_result_code;

typedef void (* qcril_uim_sap_ind_handler)(qmi_uim_sap_status_type  sap_state);
typedef void (* qcril_uim_sap_card_status_handler)(qcril_card_status_e_type state);

typedef void (* qcril_uim_sap_request_resp_handler)(int        request_id,
                                                    int        err_code,
                                                    int        qmi_err,
                                                    void     * token,
                                                    uint8_t  * data_ptr,
                                                    uint32_t   data_len);


/*===========================================================================

                            FUNCTIONS

===========================================================================*/

/*=========================================================================

FUNCTION:  qcril_uim_sap_convert_slot_id_to_slot_type

===========================================================================*/
/*!
    @brief
    Routine to convert a RIL slot id to QMI slot enum type.

    @return
    Boolean indicating the outcome of the request.
*/
/*=========================================================================*/
boolean qcril_uim_sap_convert_slot_id_to_slot_type
(
  uint32_t              slot_id,
  qmi_uim_slot_type   * qmi_slot_type_ptr
);

/*===========================================================================

FUNCTION:  qcril_uim_sap_register_client

===========================================================================*/
/*!
    @brief
    This is the SAP client indication, response, err response registeration
    function.

    @return
    None.

*/
/*=========================================================================*/
void qcril_uim_sap_register_client
(
  qcril_uim_sap_clients_type            client_id,
  qcril_uim_sap_ind_handler             ind_func_ptr,
  qcril_uim_sap_request_resp_handler    req_rsp_fun_ptr,
  qcril_uim_sap_card_status_handler     card_state_func_ptr
);

/*===========================================================================

FUNCTION:  qcril_uim_sap_send_request

===========================================================================*/
/*!
   @brief
   This function sends SAP request to qcril_uim_qcci

   @return
   int.

*/
/*=========================================================================*/
int qcril_uim_sap_send_request
(
  qcril_uim_sap_clients_type   client_id,
  qcril_uim_sap_request_type   request_type,
  int                          message_id,
  void                       * token,
  void                       * sap_req_param_ptr
);

/*===========================================================================

FUNCTION:  qcril_uim_sap_send_err_response

===========================================================================*/
/*!
    @brief
    This function queues error response to event queue

    @return
    None
*/
/*=========================================================================*/
void qcril_uim_sap_send_err_response
(
  qcril_uim_sap_clients_type   client_id,
  int                          message_id,
  void                       * token,
  int                          error_code
);

/*===========================================================================

FUNCTION:  qcril_uim_sap_get_active_client

===========================================================================*/
/*!
    @brief
    This function return active client ID

    @return
    Active client id.

*/
/*=========================================================================*/
qcril_uim_sap_clients_type qcril_uim_sap_get_active_client();

/*===========================================================================

FUNCTION:  qcril_uim_sap_set_active_client

===========================================================================*/
/*!
    @brief
    This function sets active client ID

    @return
    None.

*/
/*=========================================================================*/
void qcril_uim_sap_set_active_client(qcril_uim_sap_clients_type client_id);

/*===========================================================================

FUNCTION:  qcril_uim_sap_process_card_status

===========================================================================*/
/*!
    @brief
    This function handles the card state change indication.

    @return
    None.

*/
/*=========================================================================*/
void qcril_uim_sap_process_card_status
(
  qmi_uim_card_state_type   card_state,
  qmi_uim_card_error_type   err_type,
  uint8                     slot
);

/*===========================================================================

FUNCTION:  qcril_uim_sap_ind

===========================================================================*/
/*!
    @brief
    This is the SAP indication callback implementation for the QMI interface.

    @return
    None.

*/
/*=========================================================================*/
void qcril_uim_sap_ind
(
  qmi_uim_sap_indication_params_type   * ind_data_ptr
);

/*===========================================================================

FUNCTION:  qcril_uim_remote_server_register_service

===========================================================================*/
/*!
    @brief
    This is the uim server service registration function.

    @return
    None.

*/
/*=========================================================================*/
uint8_t qcril_uim_remote_server_register_service
(
  int instanceId
);

/*=========================================================================

FUNCTION:  qcril_uim_sap_qmi_handle_sap_connection_resp

===========================================================================*/
/*!
    @brief
    Handles the SAP connection request callback. Based on the response
    received from the modem, respective packed response types are constructed
    and the onRequestComplete is called. This completes the original request
    called on the RIL SAP socket.

    @return
    None
*/
/*=========================================================================*/
void qcril_uim_sap_qmi_handle_sap_connection_resp
(
  const qcril_uim_callback_params_type * const params_ptr
);

/*=========================================================================

FUNCTION:  qcril_uim_sap_qmi_handle_sap_request_resp

===========================================================================*/
/*!
    @brief
    Handles the SAP request callback. Based on the response
    received from the modem, respective packed response types are constructed
    and the onRequestComplete is called. This completes the original request
    called on the RIL SAP socket.

    @return
    None
*/
/*=========================================================================*/
void qcril_uim_sap_qmi_handle_sap_request_resp
(
  const qcril_uim_callback_params_type * const params_ptr
);

/*=========================================================================

FUNCTION:  qcril_uim_sap_get_sim_state

===========================================================================*/
/*!
    @brief
    Returns the current SIM state

    @return
    qcril_card_status_e_type
*/
/*=========================================================================*/
qcril_card_status_e_type qcril_uim_sap_get_sim_state();

/*=========================================================================

FUNCTION:  qcril_uim_sap_set_sim_state

===========================================================================*/
/*!
    @brief
    Sets the current SIM state

    @return
    None
*/
/*=========================================================================*/
void qcril_uim_sap_set_sim_state(qcril_card_status_e_type state);

#endif /* QCRIL_UIM_SAP_H */
