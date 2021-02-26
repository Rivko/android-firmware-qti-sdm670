/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#include "modules/nas/qcril_arb.h"
#include "modules/nas/qcril_qmi_nas.h"

#include "modules/nas/NasImsHelper.h"
#include "framework/Log.h"

#define TAG "RILQ"

static ImsServiceStatusInd::ServiceStatusInfo sImsServiceStatus;
static ImsWfcSettingsStatusInd::StatusInfo sImsWfcStatus;
static uint8_t sIsWlanRegistered = FALSE;

bool qcril_qmi_nas_ims_is_registered()
{
    switch(sImsServiceStatus.reg_status)
    {
        case ImsServiceStatusInd::RegistrationStatus::REGISTERED:
        case ImsServiceStatusInd::RegistrationStatus::LIMITED_REGISTERED:
            return true;

        default:
            return false;
    }
}

boolean qcril_qmi_nas_ims_is_ims_voip_in_service()
{
  boolean ret = FALSE;

  if (sImsServiceStatus.reg_status == ImsServiceStatusInd::RegistrationStatus::REGISTERED &&
      (sImsServiceStatus.voip_service_status != ImsServiceStatusInd::ServiceStatus::INVALID &&
       sImsServiceStatus.voip_service_status != ImsServiceStatusInd::ServiceStatus::NO_SERVICE))
  {
    ret = TRUE;
  }

  return ret;
}

uint8_t qcril_qmi_nas_ims_is_ims_registered_for_voip_vt_service()
{
  uint8_t status = FALSE;

  QCRIL_LOG_DEBUG( "IMS registered valid %d, Status %d",
      (sImsServiceStatus.reg_status  != ImsServiceStatusInd::RegistrationStatus::INVALID),
      sImsServiceStatus.reg_status);
  QCRIL_LOG_DEBUG( "VOIP service STATUS valid %d, VOIP service STATUS %d",
      (sImsServiceStatus.voip_service_status != ImsServiceStatusInd::ServiceStatus::INVALID),
      sImsServiceStatus.voip_service_status);
  QCRIL_LOG_DEBUG( "VT service STATUS valid %d, VT service STATUS %d",
      (sImsServiceStatus.vt_service_status != ImsServiceStatusInd::ServiceStatus::INVALID),
      sImsServiceStatus.vt_service_status);

  if (sImsServiceStatus.reg_status == ImsServiceStatusInd::RegistrationStatus::REGISTERED)
  {
    if(sImsServiceStatus.voip_service_status == ImsServiceStatusInd::ServiceStatus::FULL_SERVICE)
    {
      status = TRUE;
    }
    else if (sImsServiceStatus.vt_service_status == ImsServiceStatusInd::ServiceStatus::FULL_SERVICE)
    {
      status = TRUE;
    }
  }

  QCRIL_LOG_DEBUG( "IMS registered for VOIP or VT service %d", status);
  return status;
}

uint8_t qcril_qmi_nas_ims_registered_wlan_status()
{
  uint8_t status = FALSE;
  uint8_t wifi_call_valid = (sImsWfcStatus.status != ImsWfcSettingsStatusInd::Status::INVALID);
  ImsWfcSettingsStatusInd::Status wifi_call = sImsWfcStatus.status;

  QCRIL_LOG_DEBUG( "WiFi Call valid %d, WiFi Call %d", wifi_call_valid, wifi_call);

  QCRIL_LOG_DEBUG( "IMS registered valid %d, Status %d",
      (sImsServiceStatus.reg_status  != ImsServiceStatusInd::RegistrationStatus::INVALID),
      sImsServiceStatus.reg_status);
  QCRIL_LOG_DEBUG( "VOIP service STATUS valid %d, VOIP service STATUS %d",
      (sImsServiceStatus.voip_service_status != ImsServiceStatusInd::ServiceStatus::INVALID),
      sImsServiceStatus.voip_service_status);
  QCRIL_LOG_DEBUG( "VOIP service RAT valid %d, VOIP service RAT %d",
      (sImsServiceStatus.voip_service_rat != ImsServiceStatusInd::ServiceRat::INVALID),
      sImsServiceStatus.voip_service_rat);
  QCRIL_LOG_DEBUG( "VT service STATUS valid %d, VT service STATUS %d",
      (sImsServiceStatus.vt_service_status != ImsServiceStatusInd::ServiceStatus::INVALID),
      sImsServiceStatus.vt_service_status);
  QCRIL_LOG_DEBUG( "VT service RAT valid %d, VT service RAT %d",
      (sImsServiceStatus.vt_service_rat != ImsServiceStatusInd::ServiceRat::INVALID),
      sImsServiceStatus.vt_service_rat);

  if(wifi_call != ImsWfcSettingsStatusInd::Status::WFC_ON)
  {
    QCRIL_LOG_DEBUG( "WFC is NOT ON");
  }
  else if (sImsServiceStatus.reg_status == ImsServiceStatusInd::RegistrationStatus::REGISTERED)
  {
    if (sImsServiceStatus.voip_service_status == ImsServiceStatusInd::ServiceStatus::FULL_SERVICE &&
        sImsServiceStatus.voip_service_rat == ImsServiceStatusInd::ServiceRat::WLAN)
    {
      status = TRUE;
    }
    else if (sImsServiceStatus.vt_service_status == ImsServiceStatusInd::ServiceStatus::FULL_SERVICE &&
        sImsServiceStatus.vt_service_rat == ImsServiceStatusInd::ServiceRat::WLAN)
    {
      status = TRUE;
    }
  }

  QCRIL_LOG_DEBUG( "IMS registered over WLAN STATUS and also WFC is ON %d", status);
  return status;
}

uint8_t qcril_qmi_nas_ims_wlan_status()
{
  uint8_t status = FALSE;
  uint8_t wifi_call_valid = (sImsWfcStatus.status != ImsWfcSettingsStatusInd::Status::INVALID);
  ImsWfcSettingsStatusInd::Status wifi_call = sImsWfcStatus.status;
  uint8_t wifi_call_preference_valid = (sImsWfcStatus.preference != ImsWfcSettingsStatusInd::Preference::INVALID);
  ImsWfcSettingsStatusInd::Preference wifi_call_preference = sImsWfcStatus.preference;

  QCRIL_LOG_DEBUG("WiFi Call valid %d, WiFi Call %d",
      wifi_call_valid, wifi_call);
  QCRIL_LOG_DEBUG("WiFi Call pref valid %d, WiFi Call pref %d",
      wifi_call_preference_valid, wifi_call_preference);

  if (wifi_call == ImsWfcSettingsStatusInd::Status::WFC_ON) {
    if (wifi_call_preference == ImsWfcSettingsStatusInd::Preference::WLAN_PREFERRED ||
        wifi_call_preference == ImsWfcSettingsStatusInd::Preference::WLAN_ONLY) {
      status = TRUE;
    }
  }

  QCRIL_LOG_INFO("wifi_call_preference STATUS: %d", status);
  return status;
}

void qcril_qmi_nas_ims_fetch_wifi_call_info(ImsWfcSettingsStatusInd::Status *wifi_call,
    ImsWfcSettingsStatusInd::Preference *wifi_call_preference)
{
  if (wifi_call) {
    *wifi_call = sImsWfcStatus.status;
  }

  if (wifi_call_preference) {
    *wifi_call_preference = sImsWfcStatus.preference;
  }
}

static uint8_t qcril_qmi_nas_compare_ims_registered_report_unsol()
{
  uint8_t ret = FALSE;
  uint8_t old_data_rat_status = FALSE;
  uint8_t imsa_wlan_status = FALSE;
  uint8_t imss_wlan_status = FALSE;
  uint8_t dsd_wlan_status = FALSE;
  uint32_t dsd_wlan_index = QMI_RIL_ZERO;
  uint8_t reported_tech_valid = FALSE;
  int reported_tech = 0;

  QCRIL_LOG_FUNC_ENTRY();
  dsd_wlan_status = qcril_arb_check_wlan_rat_dsd_reported_helper(&dsd_wlan_index);
  old_data_rat_status = qcril_qmi_nas_fetch_data_reg_rat(&reported_tech_valid, &reported_tech);

  if(!dsd_wlan_status)
  {
    return ret;
  }

  imsa_wlan_status = qcril_qmi_nas_ims_registered_wlan_status();
  QCRIL_LOG_INFO("IMS registered over WLAN status %d", imsa_wlan_status);

  if(imsa_wlan_status)
  {
    if(!old_data_rat_status || (reported_tech != RADIO_TECH_IWLAN))
    {
      QCRIL_LOG_INFO("IMS registered over WLAN: notify ATeL as needed");
      ret = TRUE;
    }
  }
  else
  {
    imss_wlan_status = qcril_qmi_nas_ims_wlan_status();
    QCRIL_LOG_INFO("WFC available over WLAN status %d", imss_wlan_status);

    if(!imss_wlan_status && old_data_rat_status && (reported_tech == RADIO_TECH_IWLAN))
    {
      QCRIL_LOG_INFO("IMS de-registered over WLAN: notify ATeL as needed");
      ret = TRUE;
    }
  }

  return ret;
}

static uint8_t qcril_qmi_nas_compare_wfc_pref_unsol()
{
  uint8_t ret = FALSE;
  uint8_t old_data_rat_status = FALSE;
  uint8_t imss_wlan_status = FALSE;
  uint8_t imsa_wlan_status = FALSE;
  uint8_t dsd_wlan_status = FALSE;
  uint32_t dsd_wlan_index = QMI_RIL_ZERO;
  uint8_t reported_tech_valid = FALSE;
  int reported_tech;
  uint8_t wifi_call_valid = FALSE;
  ImsWfcSettingsStatusInd::Status wifi_call = ImsWfcSettingsStatusInd::Status::INVALID;
  uint8_t dsd_valid_rat_status = FALSE;
  uint32_t dsd_valid_rat_index = QMI_RIL_ZERO;

  QCRIL_LOG_FUNC_ENTRY();

  wifi_call_valid = (sImsWfcStatus.status != ImsWfcSettingsStatusInd::Status::INVALID);
  wifi_call = sImsWfcStatus.status;
  dsd_wlan_status = qcril_arb_check_wlan_rat_dsd_reported_helper(&dsd_wlan_index);
  old_data_rat_status = qcril_qmi_nas_fetch_data_reg_rat(&reported_tech_valid, &reported_tech);
  dsd_valid_rat_status = qcril_arb_find_index_rat_not_wlan_dsd_reported_helper(&dsd_valid_rat_index);

  QCRIL_LOG_INFO("WFC call valid %d, WFC status %d", wifi_call_valid, wifi_call);
  QCRIL_LOG_INFO("DSD WLAN status %d, WLAN index %d", dsd_wlan_status, dsd_wlan_index);
  QCRIL_LOG_INFO("DSD valid RAT status %d, RAT index %d", dsd_valid_rat_status, dsd_valid_rat_index);
  QCRIL_LOG_INFO("old data reg status %d, reported tech %d", old_data_rat_status, reported_tech);

  if (!dsd_wlan_status)
  {
    return ret;
  }

  imss_wlan_status = qcril_qmi_nas_ims_wlan_status();
  QCRIL_LOG_INFO("WFC available over WLAN status %d", imss_wlan_status);

  if (imss_wlan_status || ((wifi_call == ImsWfcSettingsStatusInd::Status::WFC_ON) && !dsd_valid_rat_status))
  {
    if (!old_data_rat_status || (reported_tech != RADIO_TECH_IWLAN))
    {
      QCRIL_LOG_INFO("WFC available over WLAN: notify ATeL as needed");
      ret = TRUE;
    }
  }
  else
  {
    imsa_wlan_status = qcril_qmi_nas_ims_registered_wlan_status();
    QCRIL_LOG_INFO("IMS registered over WLAN status %d", imsa_wlan_status);

    if (!imsa_wlan_status && old_data_rat_status && (reported_tech == RADIO_TECH_IWLAN))
    {
      QCRIL_LOG_INFO("WFC not available over WLAN: notify ATeL as needed");
      ret = TRUE;
    }
  }

  return ret;
}

bool qcril_qmi_nas_handle_ims_service_status_change(
    const ImsServiceStatusInd::ServiceStatusInfo &serviceStatus)
{
  bool ret = true;
  uint8_t unsol_nw_status = FALSE;

  // Update the global cache
  sImsServiceStatus = serviceStatus;
  auto isWlanReg = qcril_qmi_nas_ims_registered_wlan_status();

  if (isWlanReg != sIsWlanRegistered)
  {
    /* We need to report change in wlan reg irrespective of whether old data
       rat reported was WLAN or not - for telephony to fetch operator name */
    sIsWlanRegistered = isWlanReg;
    QCRIL_LOG_INFO(".. IMS wlan reg status changed notify UNSOL_NW_STATE_CHANGED");
    qcril_qmi_nas_wave_voice_data_status();
  }
  else if (qmi_ril_is_feature_supported(QMI_RIL_FEATURE_RIL_DATA_REG_STATE_CONDITIONAL_REPORT))
  {
    unsol_nw_status = qcril_qmi_nas_compare_ims_registered_report_unsol();
    if(unsol_nw_status)
    {
      QCRIL_LOG_INFO(".. IMS status changed notify UNSOL_NW_STATE_CHANGED");
      qcril_qmi_nas_wave_voice_data_status();
    }
  }

  if (serviceStatus.reg_status ==
      ImsServiceStatusInd::RegistrationStatus::NOT_REGISTERED)
  {
    qcril_qmi_nas_cancel_ims_deregistration_timer();
    qcril_qmi_nas_modem_shutdown_state_machine(
      QMI_RIL_NAS_MODEM_SHUTDOWN_EVENT_IMS_DEREGISTERED);
  }

  return ret;
}

bool qcril_qmi_nas_handle_ims_wfc_status_change(const ImsWfcSettingsStatusInd::StatusInfo &wfcStatusInfo)
{
  bool ret = true;
  uint8_t unsol_nw_status = FALSE;

  // Update the global cache
  sImsWfcStatus = wfcStatusInfo;

  if(qmi_ril_is_feature_supported(QMI_RIL_FEATURE_RIL_DATA_REG_STATE_CONDITIONAL_REPORT))
  {
    unsol_nw_status = qcril_qmi_nas_compare_wfc_pref_unsol();
    if(unsol_nw_status)
    {
      QCRIL_LOG_INFO(".. WFC status changed notify UNSOL_NW_STATE_CHANGED");
      qcril_qmi_nas_wave_voice_data_status();
    }
  }

  return ret;
}
