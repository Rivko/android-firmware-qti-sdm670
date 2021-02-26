/******************************************************************************
  @file    qcril_qmi_radio_config_data.c
  @brief   qcril qmi - radio config data handlers

  DESCRIPTION
    Handles data related radio config req and response handlers

  ---------------------------------------------------------------------------

  Copyright (c) 2015 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

  ---------------------------------------------------------------------------
******************************************************************************/
#include "qcril_qmi_radio_config_data.h"
#include "qcrili.h"
//----PLACEHOLDERS FOR NOW-----
/* **needs to be discussed**
 * Since this is a sync request, it will call the send response handler directly
 * which will have to take care of mapping the req-resp config items
 */
qcril_qmi_radio_config_error_type qcril_qmi_radio_config_data_set_quality_measurement_req_handler
(
  const qcril_qmi_radio_config_params_type *const config_ptr
)
{
    QCRIL_NOTUSED(config_ptr);
    return QCRIL_QMI_RADIO_CONFIG_ERROR_GENERIC_FAILURE;
}

/* **needs to be discussed**
 * Should be called from the req handler..!need to be discussed!
 */
qcril_qmi_radio_config_error_type qcril_qmi_radio_config_data_set_quality_measurement_resp_handler
(
  const qcril_qmi_radio_config_params_type *const config_ptr
)
{
    QCRIL_NOTUSED(config_ptr);
    return QCRIL_QMI_RADIO_CONFIG_ERROR_GENERIC_FAILURE;
}
