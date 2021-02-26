/******************************************************************************
#  Copyright (c) 2015, 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
/*!
  @file
  qcril_qmi_radio_config_data.h

  @brief
  Contains data related radio config structures, req and response handlers

*/


#ifndef QCRIL_QMI_RADIO_CONFIG_DATA_H
#define QCRIL_QMI_RADIO_CONFIG_DATA_H


#include "qcrili.h"
#include "qcril_qmi_radio_config_meta.h"


//----PLACEHOLDERS FOR NOW-----
/* **needs to be discussed**
 * Since this is a sync request, it will call the send response handler directly
 * which will have to take care of mapping the req-resp config items
 */
qcril_qmi_radio_config_error_type qcril_qmi_radio_config_data_set_quality_measurement_req_handler
(
  const qcril_qmi_radio_config_params_type *const config_ptr
);

/* **needs to be discussed**
 * Should be called from the req handler..!need to be discussed!
 */
qcril_qmi_radio_config_error_type qcril_qmi_radio_config_data_set_quality_measurement_resp_handler
(
  const qcril_qmi_radio_config_params_type *const config_ptr
);

#endif /* QCRIL_QMI_RADIO_CONFIG_IMSS_H */
