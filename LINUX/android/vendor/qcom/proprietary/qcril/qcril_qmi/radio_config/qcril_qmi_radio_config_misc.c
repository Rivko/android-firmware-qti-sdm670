/******************************************************************************
  @file    qcril_qmi_radio_config_misc.c
  @brief   qcril qmi - radio config misc handlers

  DESCRIPTION
    Handles miscellaneous radio config functions/structures

  ---------------------------------------------------------------------------

  Copyright (c) 2015 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

  ---------------------------------------------------------------------------
******************************************************************************/
#include "qcrili.h"
#include "qcril_log.h"
#include "qcril_qmi_radio_config_misc.h"
#include "qcril_qmi_radio_config_meta.h"


#define RADIO_CONFIG_TOKEN_SPACE (0xF0000000)

//TODO: Need to revisit this!!
//===========================================================================
// qcril_qmi_radio_config_convert_to_ril_token
//===========================================================================
RIL_Token qcril_qmi_radio_config_convert_to_ril_token(uint32_t radio_config_token)
{
  RIL_Token ret = qcril_malloc(sizeof(uint32_t));
  if (NULL != ret)
  {
    uint32_t *tmp = (uint32_t*) ret;
    *tmp = radio_config_token ^ RADIO_CONFIG_TOKEN_SPACE;
  }
  return ret;
} // qcril_qmi_radio_config_convert_to_ril_token

//===========================================================================
// qcril_qmi_radio_config_convert_ril_token_to_send
//===========================================================================
uint32_t qcril_qmi_radio_config_convert_ril_token_to_send(RIL_Token ril_token)
{
  uint32_t ret = 0xFFFFFFFF;
  if(ril_token)
  {
    ret = (*((uint32_t *) ril_token)) ^ RADIO_CONFIG_TOKEN_SPACE;
    QCRIL_LOG_INFO("radio config token: %d", ret);
    qcril_free((void*) ril_token);
  }
  else
  {
    QCRIL_LOG_ERROR("ril_token is NULL");
  }
  return ret;
}
//===========================================================================
// qcril_qmi_radio_config_map_socket_item_to_config_item
//===========================================================================
qcril_qmi_radio_config_item_type qcril_qmi_radio_config_map_socket_item_to_config_item
(
   com_qualcomm_qti_radioconfiginterface_ConfigItem config_item
)
{
  QCRIL_LOG_FUNC_ENTRY();
  //need to add all the item maps!
  qcril_qmi_radio_config_item_type ret_item = QCRIL_QMI_RADIO_CONFIG_INVALID;

  switch(config_item)
  {
    case com_qualcomm_qti_radioconfiginterface_ConfigItem_CONFIG_ITEM_IN_CALL_LTE_RSRP_LOW:
      ret_item = QCRIL_QMI_RADIO_CONFIG_DATA_QUALITY_MEASUREMENT_IN_CALL_LTE_RSRP_LOW;
      break;
    case com_qualcomm_qti_radioconfiginterface_ConfigItem_CONFIG_ITEM_IN_CALL_LTE_RSRP_MID:
      ret_item = QCRIL_QMI_RADIO_CONFIG_DATA_QUALITY_MEASUREMENT_IN_CALL_LTE_RSRP_MID;
      break;
    case com_qualcomm_qti_radioconfiginterface_ConfigItem_CONFIG_ITEM_IN_CALL_LTE_RSRP_HIGH:
      ret_item = QCRIL_QMI_RADIO_CONFIG_DATA_QUALITY_MEASUREMENT_IN_CALL_LTE_RSRP_HIGH;
      break;
    case com_qualcomm_qti_radioconfiginterface_ConfigItem_CONFIG_ITEM_IN_CALL_WIFI_RSSI_THRESHOLD_LOW:
      ret_item = QCRIL_QMI_RADIO_CONFIG_DATA_QUALITY_MEASUREMENT_IN_CALL_WIFI_RSSI_THRESHOLD_LOW;
      break;
    case com_qualcomm_qti_radioconfiginterface_ConfigItem_CONFIG_ITEM_IN_CALL_WIFI_RSSI_THRESHOLD_HIGH:
      ret_item = QCRIL_QMI_RADIO_CONFIG_DATA_QUALITY_MEASUREMENT_IN_CALL_WIFI_RSSI_THRESHOLD_HIGH;
      break;
    case com_qualcomm_qti_radioconfiginterface_ConfigItem_CONFIG_ITEM_IN_CALL_WIFI_SINR_THRESHOLD_LOW:
      ret_item = QCRIL_QMI_RADIO_CONFIG_DATA_QUALITY_MEASUREMENT_IN_CALL_WIFI_SINR_THRESHOLD_LOW;
      break;
    case com_qualcomm_qti_radioconfiginterface_ConfigItem_CONFIG_ITEM_IN_CALL_WIFI_SINR_THRESHOLD_HIGH:
      ret_item = QCRIL_QMI_RADIO_CONFIG_DATA_QUALITY_MEASUREMENT_IN_CALL_WIFI_SINR_THRESHOLD_HIGH;
      break;
    case com_qualcomm_qti_radioconfiginterface_ConfigItem_CONFIG_ITEM_IDLE_LTE_RSRP_LOW:
      ret_item = QCRIL_QMI_RADIO_CONFIG_DATA_QUALITY_MEASUREMENT_IDLE_LTE_RSRP_LOW;
      break;
    case com_qualcomm_qti_radioconfiginterface_ConfigItem_CONFIG_ITEM_IDLE_LTE_RSRP_MID:
      ret_item = QCRIL_QMI_RADIO_CONFIG_DATA_QUALITY_MEASUREMENT_IDLE_LTE_RSRP_MID;
      break;
    case com_qualcomm_qti_radioconfiginterface_ConfigItem_CONFIG_ITEM_IDLE_LTE_RSRP_HIGH:
      ret_item = QCRIL_QMI_RADIO_CONFIG_DATA_QUALITY_MEASUREMENT_IDLE_LTE_RSRP_HIGH;
      break;
    case com_qualcomm_qti_radioconfiginterface_ConfigItem_CONFIG_ITEM_IDLE_WIFI_RSSI_THRESHOLD_LOW:
      ret_item = QCRIL_QMI_RADIO_CONFIG_DATA_QUALITY_MEASUREMENT_IDLE_WIFI_RSSI_THRESHOLD_LOW;
      break;
    case com_qualcomm_qti_radioconfiginterface_ConfigItem_CONFIG_ITEM_IDLE_WIFI_RSSI_THRESHOLD_HIGH:
      ret_item = QCRIL_QMI_RADIO_CONFIG_DATA_QUALITY_MEASUREMENT_IDLE_WIFI_RSSI_THRESHOLD_HIGH;
      break;
    case com_qualcomm_qti_radioconfiginterface_ConfigItem_CONFIG_ITEM_IDLE_WIFI_SINR_THRESHOLD_LOW:
      ret_item = QCRIL_QMI_RADIO_CONFIG_DATA_QUALITY_MEASUREMENT_IDLE_WIFI_SINR_THRESHOLD_LOW;
      break;
    case com_qualcomm_qti_radioconfiginterface_ConfigItem_CONFIG_ITEM_IDLE_WIFI_SINR_THRESHOLD_HIGH:
      ret_item = QCRIL_QMI_RADIO_CONFIG_DATA_QUALITY_MEASUREMENT_IDLE_WIFI_SINR_THRESHOLD_HIGH;
      break;
    case com_qualcomm_qti_radioconfiginterface_ConfigItem_CONFIG_ITEM_ECIO_1X_THRESHOLD_LOW:
      ret_item = QCRIL_QMI_RADIO_CONFIG_DATA_QUALITY_MEASUREMENT_ECIO_1X_THRESHOLD_LOW;
      break;
    case com_qualcomm_qti_radioconfiginterface_ConfigItem_CONFIG_ITEM_ECIO_1X_THRESHOLD_HIGH:
      ret_item = QCRIL_QMI_RADIO_CONFIG_DATA_QUALITY_MEASUREMENT_ECIO_1X_THRESHOLD_HIGH;
      break;
    case com_qualcomm_qti_radioconfiginterface_ConfigItem_CONFIG_ITEM_SET_LTED_CONFIG:
      ret_item = QCRIL_QMI_RADIO_CONFIG_SET_LTED_CONFIG;
      break;
    case com_qualcomm_qti_radioconfiginterface_ConfigItem_CONFIG_ITEM_GET_LTED_CONFIG:
      ret_item = QCRIL_QMI_RADIO_CONFIG_GET_LTED_CONFIG;
      break;
    case com_qualcomm_qti_radioconfiginterface_ConfigItem_CONFIG_ITEM_SET_LTED_CATEGORY:
      ret_item = QCRIL_QMI_RADIO_CONFIG_SET_LTED_CATEGORY;
      break;
    case com_qualcomm_qti_radioconfiginterface_ConfigItem_CONFIG_ITEM_GET_LTED_CATEGORY:
      ret_item = QCRIL_QMI_RADIO_CONFIG_GET_LTED_CATEGORY;
      break;
    default:
      QCRIL_LOG_ERROR("Given item id %d cannot be mapped to internal ids! map to INVALID", config_item);
      ret_item = QCRIL_QMI_RADIO_CONFIG_INVALID;
      break;
  }

  QCRIL_LOG_INFO("Mapped radio config item from proto %d to internal item id %d", config_item, ret_item);
  QCRIL_LOG_FUNC_RETURN_WITH_RET(ret_item);
  return ret_item;
}// qcril_qmi_radio_config_map_socket_item_to_config_item


//===========================================================================
// qcril_qmi_radio_config_map_config_item_to_socket_item
//===========================================================================
com_qualcomm_qti_radioconfiginterface_ConfigItem qcril_qmi_radio_config_map_config_item_to_socket_item
(
   qcril_qmi_radio_config_item_type config_item
)
{
  QCRIL_LOG_FUNC_ENTRY();
  //need to add all the item maps!
  com_qualcomm_qti_radioconfiginterface_ConfigItem ret_item = QCRIL_QMI_RADIO_CONFIG_INVALID;

  switch(config_item)
  {
    case QCRIL_QMI_RADIO_CONFIG_DATA_QUALITY_MEASUREMENT_IN_CALL_LTE_RSRP_LOW:
      ret_item = com_qualcomm_qti_radioconfiginterface_ConfigItem_CONFIG_ITEM_IN_CALL_LTE_RSRP_LOW;
      break;
    case QCRIL_QMI_RADIO_CONFIG_DATA_QUALITY_MEASUREMENT_IN_CALL_LTE_RSRP_MID:
      ret_item = com_qualcomm_qti_radioconfiginterface_ConfigItem_CONFIG_ITEM_IN_CALL_LTE_RSRP_MID;
      break;
    case QCRIL_QMI_RADIO_CONFIG_DATA_QUALITY_MEASUREMENT_IN_CALL_LTE_RSRP_HIGH:
      ret_item = com_qualcomm_qti_radioconfiginterface_ConfigItem_CONFIG_ITEM_IN_CALL_LTE_RSRP_HIGH;
      break;
    case QCRIL_QMI_RADIO_CONFIG_DATA_QUALITY_MEASUREMENT_IN_CALL_WIFI_RSSI_THRESHOLD_LOW:
      ret_item = com_qualcomm_qti_radioconfiginterface_ConfigItem_CONFIG_ITEM_IN_CALL_WIFI_RSSI_THRESHOLD_LOW;
      break;
    case QCRIL_QMI_RADIO_CONFIG_DATA_QUALITY_MEASUREMENT_IN_CALL_WIFI_RSSI_THRESHOLD_HIGH:
      ret_item = com_qualcomm_qti_radioconfiginterface_ConfigItem_CONFIG_ITEM_IN_CALL_WIFI_RSSI_THRESHOLD_HIGH;
      break;
    case QCRIL_QMI_RADIO_CONFIG_DATA_QUALITY_MEASUREMENT_IN_CALL_WIFI_SINR_THRESHOLD_LOW:
      ret_item = com_qualcomm_qti_radioconfiginterface_ConfigItem_CONFIG_ITEM_IN_CALL_WIFI_SINR_THRESHOLD_LOW;
      break;
    case QCRIL_QMI_RADIO_CONFIG_DATA_QUALITY_MEASUREMENT_IN_CALL_WIFI_SINR_THRESHOLD_HIGH:
      ret_item = com_qualcomm_qti_radioconfiginterface_ConfigItem_CONFIG_ITEM_IN_CALL_WIFI_SINR_THRESHOLD_HIGH;
      break;
    case QCRIL_QMI_RADIO_CONFIG_DATA_QUALITY_MEASUREMENT_IDLE_LTE_RSRP_LOW:
      ret_item = com_qualcomm_qti_radioconfiginterface_ConfigItem_CONFIG_ITEM_IDLE_LTE_RSRP_LOW;
      break;
    case QCRIL_QMI_RADIO_CONFIG_DATA_QUALITY_MEASUREMENT_IDLE_LTE_RSRP_MID:
      ret_item = com_qualcomm_qti_radioconfiginterface_ConfigItem_CONFIG_ITEM_IDLE_LTE_RSRP_MID;
      break;
    case QCRIL_QMI_RADIO_CONFIG_DATA_QUALITY_MEASUREMENT_IDLE_LTE_RSRP_HIGH:
      ret_item = com_qualcomm_qti_radioconfiginterface_ConfigItem_CONFIG_ITEM_IDLE_LTE_RSRP_HIGH;
      break;
    case QCRIL_QMI_RADIO_CONFIG_DATA_QUALITY_MEASUREMENT_IDLE_WIFI_RSSI_THRESHOLD_LOW:
      ret_item = com_qualcomm_qti_radioconfiginterface_ConfigItem_CONFIG_ITEM_IDLE_WIFI_RSSI_THRESHOLD_LOW;
      break;
    case QCRIL_QMI_RADIO_CONFIG_DATA_QUALITY_MEASUREMENT_IDLE_WIFI_RSSI_THRESHOLD_HIGH:
      ret_item = com_qualcomm_qti_radioconfiginterface_ConfigItem_CONFIG_ITEM_IDLE_WIFI_RSSI_THRESHOLD_HIGH;
      break;
    case QCRIL_QMI_RADIO_CONFIG_DATA_QUALITY_MEASUREMENT_IDLE_WIFI_SINR_THRESHOLD_LOW:
      ret_item = com_qualcomm_qti_radioconfiginterface_ConfigItem_CONFIG_ITEM_IDLE_WIFI_SINR_THRESHOLD_LOW;
      break;
    case QCRIL_QMI_RADIO_CONFIG_DATA_QUALITY_MEASUREMENT_IDLE_WIFI_SINR_THRESHOLD_HIGH:
      ret_item = com_qualcomm_qti_radioconfiginterface_ConfigItem_CONFIG_ITEM_IDLE_WIFI_SINR_THRESHOLD_HIGH;
      break;
    case QCRIL_QMI_RADIO_CONFIG_DATA_QUALITY_MEASUREMENT_ECIO_1X_THRESHOLD_LOW:
      ret_item = com_qualcomm_qti_radioconfiginterface_ConfigItem_CONFIG_ITEM_ECIO_1X_THRESHOLD_LOW;
      break;
    case QCRIL_QMI_RADIO_CONFIG_DATA_QUALITY_MEASUREMENT_ECIO_1X_THRESHOLD_HIGH:
      ret_item = com_qualcomm_qti_radioconfiginterface_ConfigItem_CONFIG_ITEM_ECIO_1X_THRESHOLD_HIGH;
      break;
    case QCRIL_QMI_RADIO_CONFIG_SET_LTED_CONFIG:
      ret_item = com_qualcomm_qti_radioconfiginterface_ConfigItem_CONFIG_ITEM_SET_LTED_CONFIG;
      break;
    case QCRIL_QMI_RADIO_CONFIG_GET_LTED_CONFIG:
      ret_item = com_qualcomm_qti_radioconfiginterface_ConfigItem_CONFIG_ITEM_GET_LTED_CONFIG;
      break;
    case QCRIL_QMI_RADIO_CONFIG_SET_LTED_CATEGORY:
      ret_item = com_qualcomm_qti_radioconfiginterface_ConfigItem_CONFIG_ITEM_SET_LTED_CATEGORY;
      break;
    case QCRIL_QMI_RADIO_CONFIG_GET_LTED_CATEGORY:
      ret_item = com_qualcomm_qti_radioconfiginterface_ConfigItem_CONFIG_ITEM_GET_LTED_CATEGORY;
      break;
    default:
      QCRIL_LOG_ERROR("Given Config item %d is not present in proto! map to NONE", config_item);
      ret_item = com_qualcomm_qti_radioconfiginterface_ConfigItem_CONFIG_ITEM_NONE;
      break;
  }

  QCRIL_LOG_INFO("Mapped internal config item %d to item id %d in proto", config_item, ret_item);
  QCRIL_LOG_FUNC_RETURN_WITH_RET(ret_item);
  return ret_item;
}// qcril_qmi_radio_config_map_config_item_to_socket_item

//==============================================================================
// qcril_qmi_radio_config_imss_map_radio_config_video_quality_to_imss_vt_quality
//==============================================================================
boolean qcril_qmi_radio_config_imss_map_radio_config_video_quality_to_imss_vt_quality
(
  qcril_qmi_radio_config_imss_video_quality radio_config_video_quality,
  ims_settings_qipcall_vt_quality_enum_v01 *ims_vt_quality
)
{
  boolean ret = FALSE;
  if (ims_vt_quality)
  {
    ret = TRUE;
    switch(radio_config_video_quality)
    {
      case QCRIL_QMI_RADIO_CONFIG_IMSS_VIDEO_QUALITY_LOW:
        *ims_vt_quality = IMS_SETTINGS_VT_QUALITY_LEVEL_2_V01;
        break;
      case QCRIL_QMI_RADIO_CONFIG_IMSS_VIDEO_QUALITY_MID:
        *ims_vt_quality = IMS_SETTINGS_VT_QUALITY_LEVEL_1_V01;
        break;
      case QCRIL_QMI_RADIO_CONFIG_IMSS_VIDEO_QUALITY_HIGH:
        *ims_vt_quality = IMS_SETTINGS_VT_QUALITY_LEVEL_0_V01;
        break;
      default:
        ret = FALSE;
        break;
    }
    QCRIL_LOG_INFO("Mapped radio config video quality %d, to ims vt quality %d",
          radio_config_video_quality, *ims_vt_quality);
  }
  else
  {
    QCRIL_LOG_INFO("Invalid parameters");
  }
  return ret;
}

//==============================================================================
// qcril_qmi_radio_config_imss_map_imss_vt_quality_to_radio_config_video_quality
//==============================================================================
boolean qcril_qmi_radio_config_imss_map_imss_vt_quality_to_radio_config_video_quality
(
  ims_settings_qipcall_vt_quality_enum_v01   ims_vt_quality,
  qcril_qmi_radio_config_imss_video_quality *radio_config_video_quality
)
{
  boolean ret = FALSE;
  if (radio_config_video_quality)
  {
    ret = TRUE;
    switch(ims_vt_quality)
    {
      case IMS_SETTINGS_VT_QUALITY_LEVEL_0_V01:
        *radio_config_video_quality = QCRIL_QMI_RADIO_CONFIG_IMSS_VIDEO_QUALITY_HIGH;
        break;
      case IMS_SETTINGS_VT_QUALITY_LEVEL_1_V01:
        *radio_config_video_quality = QCRIL_QMI_RADIO_CONFIG_IMSS_VIDEO_QUALITY_MID;
        break;
      case IMS_SETTINGS_VT_QUALITY_LEVEL_2_V01:
        *radio_config_video_quality = QCRIL_QMI_RADIO_CONFIG_IMSS_VIDEO_QUALITY_LOW;
        break;
      default:
        ret = FALSE;
        break;
    }
    QCRIL_LOG_INFO("Mapped ims vt quality: %d, to radio config video quality: %d",
            ims_vt_quality, *radio_config_video_quality);
  }
  else
  {
      QCRIL_LOG_INFO("Invalid parameters");
  }
  return ret;
}
