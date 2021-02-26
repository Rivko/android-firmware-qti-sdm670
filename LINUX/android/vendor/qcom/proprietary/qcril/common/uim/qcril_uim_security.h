#ifndef QCRIL_UIM_SECURITY_H
#define QCRIL_UIM_SECURITY_H
/*===========================================================================

  Copyright (c) 2010, 2015, 2018 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //depot/asic/sandbox/users/micheleb/ril/qcril_uim_security.h#4 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/05/18   nr      New IMPI SimLock category changes
07/23/15   vr      SIMLOCK Temperory unlock status indication
03/18/15   at      Perso reactivation support
10/31/12   at      Explicit query for card status during PIN responses
06/29/10   at      Changes to support pin verification APIs
05/13/10   at      Clean up for merging with mainline
05/12/10   at      Initial version
===========================================================================*/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#include "qcril_uim.h"

/* Android property the temperory unlock status */
#define QCRIL_UIM_PROP_TEMPARORY_UNLOCK_STATUS         "persist.vendor.radio.uim_temp_unlock"
#define QCRIL_UIM_PROP_TEMPARORY_UNLOCK_STATUS_TRUE    "1"
#define QCRIL_UIM_PROP_TEMPARORY_UNLOCK_STATUS_FALSE   "0"

/* Personalization: SPN and SP_EHPLMN feature
   The values are assigned based on RIL_PersoSubstate enum in Ril.h file
   These should be removed after defining in Ril.h file */
#define RIL_PERSOSUBSTATE_SIM_SPN                      100
#define RIL_PERSOSUBSTATE_SIM_SPN_PUK                  101
#define RIL_PERSOSUBSTATE_SIM_SP_EHPLMN                102
#define RIL_PERSOSUBSTATE_SIM_SP_EHPLMN_PUK            103
#define RIL_PERSOSUBSTATE_SIM_ICCID                    104
#define RIL_PERSOSUBSTATE_SIM_ICCID_PUK                105
#define RIL_PERSOSUBSTATE_SIM_IMPI                     106
#define RIL_PERSOSUBSTATE_SIM_IMPI_PUK                 107

/* New substate to inform the application about perso locked with no ck */
#define RIL_PERSOSUBSTATE_SIM_RSU_NO_CK                200

typedef enum
{
  QCRIL_UIM_REMOTE_SIMLOCK_GENERATE_ENCRYPTED_KEY,
  QCRIL_UIM_REMOTE_SIMLOCK_PROCESS_SIMLOCK_DATA,
  QCRIL_UIM_REMOTE_SIMLOCK_GENERATE_HMAC,
  QCRIL_UIM_REMOTE_SIMLOCK_GET_MAX_SUPPORTED_VERSION,
  QCRIL_UIM_REMOTE_SIMLOCK_GET_STATUS,
  QCRIL_UIM_REMOTE_SIMLOCK_GENERATE_BLOB,
  QCRIL_UIM_REMOTE_SIMLOCK_START_UNLOCK_TIMER,
  QCRIL_UIM_REMOTE_SIMLOCK_STOP_UNLOCK_TIMER
} qcril_uim_remote_simlock_operation_type;

typedef enum
{
  QCRIL_UIM_REMOTE_SIMLOCK_RESP_SUCCESS,
  QCRIL_UIM_REMOTE_SIMLOCK_RESP_FAILURE,
  QCRIL_UIM_REMOTE_SIMLOCK_RESP_GET_TIME_FAILED,
  QCRIL_UIM_REMOTE_SIMLOCK_RESP_TIMER_EXPIRED
} qcril_uim_remote_simlock_response_type;

typedef enum
{
  QCRIL_UIM_REMOTE_SIMLOCK_STATE_LOCKED,
  QCRIL_UIM_REMOTE_SIMLOCK_STATE_TEMPERORY_UNLOCK,
  QCRIL_UIM_REMOTE_SIMLOCK_STATE_PERMANENT_UNLOCK
} qcril_uim_remote_simlock_status_type;

typedef struct
{
  uint8_t  majorVersion;
  uint8_t  minorVersion;
} qcril_uim_remote_simlock_version;

typedef struct UimRemoteSimlockStatus
{
  qcril_uim_remote_simlock_status_type  status;
  uint32_t                              unlockTime;
} qcril_uim_remote_simlock_status;

/*===========================================================================

  FUNCTION:  qcril_uim_pin_resp

===========================================================================*/
/*!
    @brief
    Handle PIN operation confirmations

    @return
    None
*/
/*=========================================================================*/
void qcril_uim_pin_resp
(
  qcril_uim_callback_params_type             * params_ptr,
  qcril_request_return_type                  * const ret_ptr
);


/*===========================================================================

  FUNCTION:  qcril_uim_deperso_resp

===========================================================================*/
/*!
    @brief
    Handle deperso operation confirmation

    @return
    None
*/
/*=========================================================================*/
void qcril_uim_deperso_resp
(
  qcril_uim_callback_params_type  * params_ptr
);


/*===========================================================================

  FUNCTION:  qcril_uim_perso_resp

===========================================================================*/
/*!
    @brief
    Handle perso operation confirmation

    @return
    None
*/
/*=========================================================================*/
void qcril_uim_perso_resp
(
  qcril_uim_callback_params_type  * params_ptr
);

/*===========================================================================

  FUNCTION:  qcril_uim_remote_unlock_resp

===========================================================================*/
/*!
    @brief
    Handle remote unlock response

    @return
    None
*/
/*=========================================================================*/
void qcril_uim_remote_unlock_resp
(
  qcril_uim_callback_params_type  * params_ptr
);

/*===========================================================================

  FUNCTION:  qcril_uim_request_set_pin_status

===========================================================================*/
/*!
    @brief
    Handles QCRIL_EVT_INTERNAL_MMGSDI_SET_PIN1_STATUS request from
    the framework

    @return
    None
*/
/*=========================================================================*/
void qcril_uim_request_set_pin_status
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type       *const ret_ptr /*!< Output parameter */
);


/*===========================================================================

  FUNCTION:  qcril_uim_request_get_pin_status

===========================================================================*/
/*!
    @brief
    Handles QCRIL_EVT_INTERNAL_MMGSDI_GET_PIN1_STATUS requests
    from the framework

    @return
    None
*/
/*=========================================================================*/
void qcril_uim_request_get_pin_status
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type       *const ret_ptr /*!< Output parameter */
);

/*===========================================================================

  FUNCTION:  qcril_uim_process_simlock_temp_unlock_ind

===========================================================================*/
/*!
    @brief
    Main function for processing QMI SIMLOCK temporary unlock indication.

    @return
    None.
*/
/*=========================================================================*/
void qcril_uim_process_simlock_temp_unlock_ind
(
  const qcril_uim_indication_params_type  * ind_param_ptr,
  qcril_request_return_type               * const ret_ptr /*!< Output parameter */
);

/*===========================================================================

  FUNCTION:  qcril_uim_check_and_send_temp_unlock_expiry_ind

===========================================================================*/
/*!
    @brief
    Function to check the temperory unlock status and send expiry indication

    @return
    None.
*/
/*=========================================================================*/
void qcril_uim_check_and_send_temp_unlock_expiry_ind
(
  void
);

/*===========================================================================

FUNCTION:  qcril_uim_remote_simlock_response

===========================================================================*/
void qcril_uim_remote_simlock_response
(
  void                                    * token_ptr,
  qcril_uim_remote_simlock_response_type    response,
  qcril_uim_remote_simlock_operation_type   op_type,
  uint8                                   * simlock_rsp_ptr,
  uint32                                    simlock_rsp_len,
  qcril_uim_remote_simlock_version          version,
  qcril_uim_remote_simlock_status           status,
  int                                       timer_value
);

/*=========================================================================

  FUNCTION:  qcril_uim_request_remote_sim_lock_unlock

===========================================================================*/
/*!
    @brief
    Handles QCRIL_EVT_HOOK_REMOTE_SIM_LOCK_UNLOCK request from QCRIL.

    @return
    None
*/
/*=========================================================================*/
void qcril_uim_request_remote_sim_lock_unlock
(
  void                                     * token_ptr,
  qcril_uim_remote_simlock_operation_type    op_type,
  const uint8_t                            * simlock_data_ptr,
  uint32_t                                   simlock_len
);
#endif /* QCRIL_UIM_SECURITY_H */

