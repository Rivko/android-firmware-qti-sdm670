/*!
  @file
  qcril_qmi_radio_config_data.h

  @brief
  Contains data related radio config structures, req and response handlers

*/

/*===========================================================================

  Copyright (c) 2015 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

===========================================================================*/

#ifndef QCRIL_QMI_RADIO_CONFIG_DATA_H
#define QCRIL_QMI_RADIO_CONFIG_DATA_H


#include "qcrili.h"
#include "qcril_qmi_radio_config_meta.h"

/**
 * This structure is sent to qcril_data which
 * corresponds to dsd_set_quality_measurement request
 */
typedef struct {

  /* Optional */
  /*  In call LTE Low Threshold */
  uint8_t in_call_lte_rsrp_low_valid;
  int16_t in_call_lte_rsrp_low;

  /* Optional */
  /*  In call LTE Mid Threshold */
  uint8_t in_call_lte_rsrp_mid_valid;
  int16_t in_call_lte_rsrp_mid;

  /* Optional */
  /*  In call LTE High Threshold */
  uint8_t in_call_lte_rsrp_high_valid;
  int16_t in_call_lte_rsrp_high;

  /* Optional */
  /*  In call Wifi RSSI Low threshold param */
  uint8_t in_call_wifi_rssi_threshold_low_valid;
  int16_t in_call_wifi_rssi_threshold_low;

  /* Optional */
  /*  In call Wifi RSSI High threshold param */
  uint8_t in_call_wifi_rssi_threshold_high_valid;
  int16_t in_call_wifi_rssi_threshold_high;

  /* Optional */
  /*  In call Wifi SINR Low threshold param */
  uint8_t in_call_wifi_sinr_threshold_low_valid;
  int16_t in_call_wifi_sinr_threshold_low;

  /* Optional */
  /*  In call Wifi SINR High threshold param */
  uint8_t in_call_wifi_sinr_threshold_high_valid;
  int16_t in_call_wifi_sinr_threshold_high;

  /* Optional */
  /*  Idle LTE Threshold Low param */
  uint8_t idle_lte_rsrp_low_valid;
  int16_t idle_lte_rsrp_low;

  /* Optional */
  /*  Idle LTE Threshold Mid param */
  uint8_t idle_lte_rsrp_mid_valid;
  int16_t idle_lte_rsrp_mid;

  /* Optional */
  /*  Idle LTE Threshold High param */
  uint8_t idle_lte_rsrp_high_valid;
  int16_t idle_lte_rsrp_high;

  /* Optional */
  /*  Idle Wifi RSSI threshold Low param */
  uint8_t idle_wifi_rssi_threshold_low_valid;
  int16_t idle_wifi_rssi_threshold_low;

  /* Optional */
  /*  Idle Wifi RSSI threshold High param */
  uint8_t idle_wifi_rssi_threshold_high_valid;
  int16_t idle_wifi_rssi_threshold_high;

  /* Optional */
  /*  Idle Wifi SINR threshold Low param */
  uint8_t idle_wifi_sinr_threshold_low_valid;
  int16_t idle_wifi_sinr_threshold_low;

  /* Optional */
  /*  Idle Wifi SINR threshold High param */
  uint8_t idle_wifi_sinr_threshold_high_valid;
  int16_t idle_wifi_sinr_threshold_high;

  /* Optional */
  /*  1x threshold Low param */
  uint8_t ecio_1x_threshold_low_valid;
  int16_t ecio_1x_threshold_low;

  /* Optional */
  /*  1x threshold High param */
  uint8_t ecio_1x_threshold_high_valid;
  int16_t ecio_1x_threshold_high;

} qcril_qmi_radio_config_data_quality_measurement_req_type;


typedef struct {
    qmi_response_type_v01 resp;
} qcril_qmi_radio_config_data_resp_type;


/**
 * API to communicate the quality measurement threshold
 * values to qcril_data.
 *
 * @param params
 *    qcril_qmi_radio_config_data_quality_measurement_req_type
 *
 * @return qcril_qmi_radio_config_data_resp_type
 */
qcril_qmi_radio_config_data_resp_type qcril_data_set_quality_measurement
(
   qcril_qmi_radio_config_data_quality_measurement_req_type* params
);

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
