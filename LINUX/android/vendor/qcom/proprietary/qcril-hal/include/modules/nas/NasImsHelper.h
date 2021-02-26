/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#pragma once

#include "modules/ims/ImsServiceStatusInd.h"
#include "modules/ims/ImsWfcSettingsStatusInd.h"

boolean qcril_qmi_nas_ims_is_ims_voip_in_service();
uint8_t qcril_qmi_nas_ims_is_ims_registered_for_voip_vt_service();
uint8_t qcril_qmi_nas_ims_wlan_status();
uint8_t qcril_qmi_nas_ims_registered_wlan_status();
void qcril_qmi_nas_ims_fetch_wifi_call_info(ImsWfcSettingsStatusInd::Status *wifi_call,
    ImsWfcSettingsStatusInd::Preference *wifi_call_preference);
bool qcril_qmi_nas_handle_ims_service_status_change(
    const ImsServiceStatusInd::ServiceStatusInfo &serviceStatus);
bool qcril_qmi_nas_handle_ims_wfc_status_change(const ImsWfcSettingsStatusInd::StatusInfo &statusInfo);
bool qcril_qmi_nas_ims_is_registered();
