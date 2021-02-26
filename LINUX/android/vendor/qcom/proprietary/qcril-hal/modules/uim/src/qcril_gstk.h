#ifndef QCRIL_GSTK_QMI_H
#define QCRIL_GSTK_QMI_H
/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#  ---------------------------------------------------------------------------
#******************************************************************************/
/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
#include "modules/uim/GstkRilRequestMessage.h"
#include "modules/uim/GstkQmiCatIndicationMsg.h"
#include "GstkQmiCatResponseMsg.h"

/*===========================================================================
  FUNCTION:  qcril_gstk_reset_state
===========================================================================*/
/*!
  @brief
    Reset GSTK.

  @return
    Nothing

  @msc
  @endmsc
*/
/*=========================================================================*/
void qcril_gstk_reset_state
(
  boolean is_refresh
);

/*===========================================================================
  FUNCTION:  qcril_gstk_init_state
===========================================================================*/
/*!
  @brief
    Init GSTK.

  @return
    Nothing

  @msc
  @endmsc
*/
/*=========================================================================*/
int qcril_gstk_init_state
(
  void
);

/*===========================================================================
  FUNCTION:  qcril_gstk_qmi_request_stk_service_is_running
===========================================================================*/
/*!
  @brief
    Handles RIL_REQUEST_REPORT_STK_SERVICE_IS_RUNNING

  @param[in]  req_ptr  Android RIL request message

  @return
    RIL_Errno

  @return
    Nothing
*/
/*=========================================================================*/
RIL_Errno qcril_gstk_qmi_request_stk_service_is_running
(
  std::shared_ptr<GstkRilRequestMessage> req_ptr,
  bool                                   m_ready
);

/*===========================================================================
  FUNCTION:  qcril_gstk_qmi_request_stk_handle_call_setup_requested_from_sim
===========================================================================*/
/*!
  @brief
    Handles RIL_REQUEST_STK_HANDLE_CALL_SETUP_REQUESTED_FROM_SIM.

    This request arrives from the SIM Toolkit application after the user
    has accepted or rejected a call originating from a SETUP_CALL command,
    and the request data indicates which.

  @param[in]  req_ptr  Android RIL request message

  @return
    RIL_Errno
*/
/*=========================================================================*/
RIL_Errno qcril_gstk_qmi_request_stk_handle_call_setup_requested_from_sim
(
  std::shared_ptr<GstkRilRequestMessage> req_ptr
);

/*===========================================================================
  FUNCTION:  qcril_gstk_qmi_request_stk_send_terminal_response
===========================================================================*/
/*!
  @brief
    Handles RIL_REQUEST_STK_SEND_TERMINAL_RESPONSE

    For proactive commands for which the Terminal Response originates from
    the Android Toolkit application, this function arranges for that
    response to be sent to the card.

  @param[in]  req_ptr  Android RIL request message

  @return
    RIL_Errno
*/
/*=========================================================================*/
RIL_Errno qcril_gstk_qmi_request_stk_send_terminal_response
(
  std::shared_ptr<GstkRilRequestMessage> req_ptr
);

/*===========================================================================

FUNCTION:  qcril_gstk_qmi_request_stk_send_envelope_command
===========================================================================*/
/*!
  @brief
    Handles RIL_REQUEST_STK_SEND_ENVELOPE_COMMAND.

  @param[in]  req_ptr  Android RIL request message

  @return
    RIL_Errno

  @msc
  @endmsc

  @see
*/
/*=========================================================================*/
RIL_Errno qcril_gstk_qmi_request_stk_send_envelope_command
(
  std::shared_ptr<GstkRilRequestMessage> req_ptr
);

/*===========================================================================

FUNCTION:  qcril_gstk_process_qmi_cat_ind
===========================================================================*/
/*!
  @brief
    Handles QMI CAT indications.

  @param[in]  ind_id  Qmi Cat indication id
  @param[in]  ind_ptr Qmi cat indication message

  @return
    None

  @msc
  @endmsc

  @see
*/
/*=========================================================================*/
void qcril_gstk_process_qmi_cat_ind
(
  qmi_cat_indication_id_type       ind_id,
  qcril_gstk_qmi_ind_params_type  *ind_ptr
);

/*===========================================================================

FUNCTION:  qcril_gstk_process_qmi_cat_cb
===========================================================================*/
/*!
  @brief
    Handles QMI CAT async callback.

  @param[in]  cb_ptr Qmi cat callback message

  @return
    None

  @msc
  @endmsc

  @see
*/
/*=========================================================================*/
void qcril_gstk_process_qmi_cat_cb
(
  qmi_cat_rsp_data_type  *cb_ptr
);

#endif /* QCRIL_GSTK_QMI_H */
