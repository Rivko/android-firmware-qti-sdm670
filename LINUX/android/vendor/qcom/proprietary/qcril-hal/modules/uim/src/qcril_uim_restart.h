/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#ifndef QCRIL_UIM_RESTART_H
#define QCRIL_UIM_RESTART_H

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
#include <telephony/ril.h>
#include "modules/uim/qcril_uim.h"


/*=========================================================================

  FUNCTION:  qcril_uim_store_encrypted_pin

===========================================================================*/
/*!
    @brief
    Stores encrypted PIN1 along with ICCID and application ID.

    @return
    None.
*/
/*=========================================================================*/
void qcril_uim_store_encrypted_pin
(
  qmi_uim_rsp_data_type                        *rsp_data_ptr,
  qmi_uim_session_type                         session_type
);


/*=========================================================================

  FUNCTION:  qcril_uim_try_pin1_verification

===========================================================================*/
/*!
    @brief
    Checks ICCID, AID and PIN1 status before sending PIN1 for verification
    after modem restart.

    @return
    Status of PIN1 verification.
*/
/*=========================================================================*/
RIL_Errno qcril_uim_try_pin1_verification
(
   uint8                                       slot
);


/*=========================================================================

  FUNCTION:  qcril_uim_clear_encrypted_pin

===========================================================================*/
/*!
    @brief
    Clears cached value of encrypted PIN1.

    @return
    None.
*/
/*=========================================================================*/
void qcril_uim_clear_encrypted_pin
(
  qmi_uim_session_type                         session_type
);


/*=========================================================================

  FUNCTION:  qcril_uim_check_silent_pin_verify_in_progress

===========================================================================*/
/*!
    @brief
    Function to check if silent pin1 verification is in progress. Resets
    the flag if PIN1 state is ENABLED_VERIFIED.

    @return
    None.
*/
/*=========================================================================*/
void qcril_uim_check_silent_pin_verify_in_progress
(
  uint8                                       slot
);


/*=========================================================================

  FUNCTION:  qcril_uim_clear_encrypted_pin_after_card_reset

===========================================================================*/
/*!
    @brief
    Clears encrypted PIN1 data after card reset.

    @return
    None.
*/
/*=========================================================================*/
void qcril_uim_clear_encrypted_pin_after_card_reset
(
  uint8                                        slot
);


/*=========================================================================

  FUNCTION:  qcril_uim_process_modem_restart_start

===========================================================================*/
/*!
    @brief
    Resets state of QCRIL_UIM, sends card error events to QCRIL, and sends
    unsolicited response to Android framework for change in card status.
    Called as a result of QCRIL_EVT_INTERNAL_MMGSDI_MODEM_RESTART_START.

    @return
    None.
*/
/*=========================================================================*/
void qcril_uim_process_modem_restart_start
(
  const qcril_request_params_type            * const params_ptr,
  qcril_request_return_type                  * const ret_ptr
);


/*=========================================================================

  FUNCTION:  qcril_uim_process_modem_restart_complete

===========================================================================*/
/*!
    @brief
    Initializes state of QCRIL_UIM and sets global to indicate that the
    modem has restarted. Called as a result of
    QCRIL_EVT_INTERNAL_MMGSDI_MODEM_RESTART_COMPLETE.

    @return
    None.
*/
/*=========================================================================*/
void qcril_uim_process_modem_restart_complete
(
  const qcril_request_params_type            * const params_ptr,
  qcril_request_return_type                  * const ret_ptr
);


/*===========================================================================

  FUNCTION:  qcril_uim_is_silent_pin_verification_needed

===========================================================================*/
/*!
    @brief
    Checks if pin needs to be verified silently for any application on the given slot.

    @return
    TRUE if silent pin verification is needed, else FALSE
*/
/*=========================================================================*/
boolean qcril_uim_is_silent_pin_verification_needed
(
  uint8                  slot
);

#endif /* QCRIL_UIM_RESTART_H */
