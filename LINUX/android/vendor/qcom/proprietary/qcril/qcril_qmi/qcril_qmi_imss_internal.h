/******************************************************************************
  @file    qcril_qmi_imss_internal.h
  @brief   qcril qmi - IMS Setting

  DESCRIPTION
    QCRIL IMSS internal types and functions.

  ---------------------------------------------------------------------------

  Copyright (c) 2015, 2016 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

  ---------------------------------------------------------------------------
******************************************************************************/

#ifndef QCRIL_QMI_IMSS_INTERNAL_H
#define QCRIL_QMI_IMSS_INTERNAL_H

//===========================================================================
//                    INTERNAL DEFINITIONS AND TYPES
//===========================================================================
typedef enum
{
  QCRIL_IMS_SETTING_WFC_NOT_SUPPORTED = 0,
  QCRIL_IMS_SETTING_WFC_OFF = 1,
  QCRIL_IMS_SETTING_WFC_ON = 2
} qcril_ims_setting_wfc_status_type;

typedef enum
{
  QCRIL_IMS_SETTING_WFC_WLAN_PREFERRED = 0,
  QCRIL_IMS_SETTING_WFC_WLAN_ONLY = 1,
  QCRIL_IMS_SETTING_WFC_CELLULAR_PREFERRED = 2,
  QCRIL_IMS_SETTING_WFC_CELLULAR_ONLY = 3,
  QCRIL_IMS_SETTING_WFC_PREF_NONE = 4
} qcril_ims_setting_wfc_preference_type;

struct ims_cached_info_type
{
  pthread_mutex_t       imss_info_lock_mutex;
  pthread_mutexattr_t   imss_info_lock_mutex_atr;

  uint8_t imss_reg_state_valid;
  ims_Registration_RegState imss_reg_state;

  uint8_t imss_state_change_requested;
  ims_Registration_RegState imss_new_reg_state;

  uint8_t wifi_calling_enabled_valid;
  qcril_ims_setting_wfc_status_type wifi_calling_enabled;

  uint8_t call_mode_preference_valid;
  qcril_ims_setting_wfc_preference_type call_mode_preference;

  uint8_t service_mask_by_network_enabled_valid;
  ims_settings_omadm_services_type_mask_v01 service_mask_by_network_enabled;
};

extern struct ims_cached_info_type   qcril_qmi_ims_cached_info;
#endif
