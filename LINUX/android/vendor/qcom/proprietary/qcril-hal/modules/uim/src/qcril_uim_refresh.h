#ifndef QCRIL_UIM_REFRESH_H
#define QCRIL_UIM_REFRESH_H
/*===========================================================================

  Copyright (c) 2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

===========================================================================*/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#include "modules/uim/qcril_uim.h"

/*=========================================================================

  FUNCTION:  qcril_uim_refresh_register_resp

===========================================================================*/
/*!
    @brief
    Process the response for refresh register.

    @return
    None
*/
/*=========================================================================*/
void qcril_uim_refresh_register_resp
(
  const qcril_uim_callback_params_type * const params_ptr
);


/*===========================================================================

  FUNCTION:  qcril_uim_init_refresh_info

===========================================================================*/
/*!
    @brief
    Initializes the global refresh info structure.

    @return
    None.
*/
/*=========================================================================*/
void qcril_uim_init_refresh_info
(
  void
);


/*===========================================================================

  FUNCTION:  qcril_uim_cleanup_refresh_info

===========================================================================*/
/*!
    @brief
    Frees the memory in global refresh info structure.

    @return
    None.
*/
/*=========================================================================*/
void qcril_uim_cleanup_refresh_info
(
  void
);


/*===========================================================================

  FUNCTION:  qcril_uim_refresh_register

===========================================================================*/
/*!
    @brief
    Checks if GW and Primary subscription Apps are in ready state. If yes,
    it Prepares the data needed for refresh register QMI command and sends it.

    @return
    None.
*/
/*=========================================================================*/
void qcril_uim_refresh_register
(
  qmi_uim_session_type                qmi_session_type
);


#ifdef FEATURE_QCRIL_UIM_ISIM_REFRESH
/*===========================================================================

  FUNCTION:  qcril_uim_refresh_register_isim

===========================================================================*/
/*!
    @brief
    Registers for refresh for an ISIM App.

    @return
    None.
*/
/*=========================================================================*/
void qcril_uim_refresh_register_isim
(
  qmi_uim_slot_type                   qmi_slot
);
#endif /* FEATURE_QCRIL_UIM_ISIM_REFRESH */


/*===========================================================================

  FUNCTION:  qcril_uim_process_refresh_ind

===========================================================================*/
/*!
    @brief
    Main function for processing QMI refresh indications. Based
    on the indication received, if needed, it updates the global card status,
    ret_ptr and sends card events internal to QCRIL (CM & PBM).

    @return
    None.
*/
/*=========================================================================*/
void qcril_uim_process_refresh_ind
(
  const qcril_uim_indication_params_type  * ind_param_ptr,
  qcril_request_return_type               * const ret_ptr   /*!< Output parameter */
);


/*=========================================================================

  FUNCTION:  qcril_uim_refresh_read_pbr_record_resp

===========================================================================*/
/*!
    @brief
    Process the response for PBR read

    @return
    None
*/
/*=========================================================================*/
void qcril_uim_refresh_read_pbr_record_resp
(
  const qcril_uim_callback_params_type * const params_ptr
);


/*=========================================================================

  FUNCTION:  qcril_uim_refresh_get_pbr_attr_resp

===========================================================================*/
/*!
    @brief
    Process the response for get PBR attributes.

    @return
    None
*/
/*=========================================================================*/
void qcril_uim_refresh_get_pbr_attr_resp
(
  const qcril_uim_callback_params_type * const params_ptr
);

/*===========================================================================

  FUNCTION:   qcril_uim_refresh_register_card_session

===========================================================================*/
/*!
    @brief
    Registers for refresh for card session.

    @return
    None.
*/
/*=========================================================================*/
void qcril_uim_refresh_register_card_session
(
  uint8                   slot
);

#endif /* QCRIL_UIM_REFRESH_H */

