/******************************************************************************
#  Copyright (c) 2015, 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
/*!
  @file
  qcril_qmi_radio_config_misc.h

  @brief
  Contains miscellaneous functions/structures needed for eadio config

*/

#ifndef QCRIL_QMI_RADIO_CONFIG_MISC_H
#define QCRIL_QMI_RADIO_CONFIG_MISC_H

#include "qcrili.h"
#include "telephony/ril.h"
#include "qcril_qmi_radio_config_meta.h"
#include "qcril_qmi_radio_config_imss.h"
#include "radio_config_interface.pb.h"

#ifdef __cplusplus
extern "C" {
#endif

RIL_Token qcril_qmi_radio_config_convert_to_ril_token(uint32_t radio_config_token);

uint32_t qcril_qmi_radio_config_convert_ril_token_to_send(RIL_Token token);

qcril_qmi_radio_config_item_type qcril_qmi_radio_config_map_socket_item_to_config_item
(
   com_qualcomm_qti_radioconfiginterface_ConfigItem config_item
);

com_qualcomm_qti_radioconfiginterface_ConfigItem qcril_qmi_radio_config_map_config_item_to_socket_item
(
   qcril_qmi_radio_config_item_type config_item
);

boolean qcril_qmi_radio_config_imss_map_radio_config_video_quality_to_imss_vt_quality
(
  qcril_qmi_radio_config_imss_video_quality radio_config_video_quality,
  ims_settings_qipcall_vt_quality_enum_v01 *ims_vt_quality
);

boolean qcril_qmi_radio_config_imss_map_imss_vt_quality_to_radio_config_video_quality
(
  ims_settings_qipcall_vt_quality_enum_v01   ims_vt_quality,
  qcril_qmi_radio_config_imss_video_quality *radio_config_video_quality
);

#ifdef __cplusplus
}
#endif
#endif
