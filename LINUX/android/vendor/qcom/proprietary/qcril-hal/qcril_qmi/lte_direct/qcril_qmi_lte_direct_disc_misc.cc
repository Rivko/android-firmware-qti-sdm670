/******************************************************************************
#  Copyright (c) 2016-2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
/******************************************************************************
  @file    qcril_qmi_lte_direct_disc_misc.c
  @brief   qcril qmi - lte direct discovery misc

  DESCRIPTION
    Utility functions for lte direct discovery

******************************************************************************/

/*===========================================================================
                           INCLUDE FILES
===========================================================================*/

#include "qcril_qmi_lte_direct_disc_misc.h"
#include "qcril_log.h"
#include "qcrili.h"
#include "lte_v01.h"
#include <framework/legacy.h>
#include <map>

/*===========================================================================
                    INTERNAL DEFINITIONS AND TYPES
===========================================================================*/
#define LTED_TOKEN_SPACE (0xE9000000)

static std::map<qcril_evt_e_type, LteDirectDiscovery_MsgId> qcril_qmi_lte_direct_disc_event_map;
/*===========================================================================
                                FUNCTIONS
===========================================================================*/

void qcril_qmi_lte_direct_disc_event_map_init()
{
    qcril_qmi_lte_direct_disc_event_map[QCRIL_EVT_LTE_DIRECT_DISC_REQ_INITIALIZE] =
                                            LteDirectDiscovery_MsgId_REQUEST_INITIALIZE;
    qcril_qmi_lte_direct_disc_event_map[QCRIL_EVT_LTE_DIRECT_DISC_REQ_GET_DEVICE_CAPABILITY] =
                                            LteDirectDiscovery_MsgId_REQUEST_GET_DEVICE_CAPABILITY;
    qcril_qmi_lte_direct_disc_event_map[QCRIL_EVT_LTE_DIRECT_DISC_REQ_TERMINATE] =
                                            LteDirectDiscovery_MsgId_REQUEST_TERMINATE;
    qcril_qmi_lte_direct_disc_event_map[QCRIL_EVT_LTE_DIRECT_DISC_REQ_GET_SERVICE_STATUS] =
                                            LteDirectDiscovery_MsgId_REQUEST_GET_SERVICE_STATUS;
    qcril_qmi_lte_direct_disc_event_map[QCRIL_EVT_LTE_DIRECT_DISC_REQ_PUBLISH] =
                                            LteDirectDiscovery_MsgId_REQUEST_PUBLISH;
    qcril_qmi_lte_direct_disc_event_map[QCRIL_EVT_LTE_DIRECT_DISC_REQ_CANCEL_PUBLISH] =
                                            LteDirectDiscovery_MsgId_REQUEST_CANCEL_PUBLISH;
    qcril_qmi_lte_direct_disc_event_map[QCRIL_EVT_LTE_DIRECT_DISC_REQ_SUBSCRIBE] =
                                            LteDirectDiscovery_MsgId_REQUEST_SUBSCRIBE;
    qcril_qmi_lte_direct_disc_event_map[QCRIL_EVT_LTE_DIRECT_DISC_REQ_CANCEL_SUBSCRIBE] =
                                            LteDirectDiscovery_MsgId_REQUEST_CANCEL_SUBSCRIBE;
} // qcril_qmi_lte_direct_disc_map_event_to_request

//===========================================================================
// qcril_qmi_lte_direct_disc_convert_lted_token_to_ril_token
//===========================================================================
RIL_Token qcril_qmi_lte_direct_disc_convert_lted_token_to_ril_token
(
 uint32_t lted_token
)
{
  RIL_Token ret = qcril_malloc(sizeof(uint32_t));
  if (NULL != ret)
  {
    uint32_t *tmp = (uint32_t*) ret;
    *tmp = lted_token ^ LTED_TOKEN_SPACE;
  }
  return ret;
} // qcril_qmi_lte_direct_disc_convert_lted_token_to_ril_token

//===========================================================================
// qcril_qmi_lte_direct_disc_free_and_convert_ril_token_to_lted_token
//===========================================================================
uint32_t qcril_qmi_lte_direct_disc_free_and_convert_ril_token_to_lted_token
(
 RIL_Token ril_token
)
{
  uint32_t ret = 0xFFFFFFFF;
  if (ril_token)
  {
    ret = (*((uint32_t *) ril_token)) ^ LTED_TOKEN_SPACE;
    QCRIL_LOG_INFO("lted token: %d", ret);
    qcril_free((void*) ril_token);
  }
  else
  {
    QCRIL_LOG_INFO("ril_token is NULL");
  }

  return ret;
} // qcril_qmi_lte_direct_disc_free_and_convert_ril_token_to_lted_token

//===========================================================================
// qcril_qmi_lte_direct_disc_map_qmi_error_to_lted_error
//===========================================================================
LteDirectDiscovery_Error qcril_qmi_lte_direct_disc_map_qmi_error_to_lted_error
(
 qmi_response_type_v01* qmi_service_response
)
{
  LteDirectDiscovery_Error res = LteDirectDiscovery_Error_E_GENERIC_FAILURE;

  if (qmi_service_response == NULL)
  {
    res = LteDirectDiscovery_Error_E_SUCCESS;
  }
  else
  {
    switch (qmi_service_response->result)
    {
      case QMI_RESULT_SUCCESS_V01:
        res = LteDirectDiscovery_Error_E_SUCCESS;
        break;
      case QMI_RESULT_FAILURE_V01:
        switch (qmi_service_response->error)
        {
          case QMI_ERR_NONE_V01:
            res = LteDirectDiscovery_Error_E_SUCCESS;
            break;
          case QMI_ERR_INTERNAL_V01:
          case QMI_ERR_MALFORMED_MSG_V01:
          case QMI_ERR_MISSING_ARG_V01:
          case QMI_ERR_INVALID_ARG_V01:
          case QMI_ERR_INVALID_TX_ID_V01:
            res = LteDirectDiscovery_Error_E_GENERIC_FAILURE;
            break;
          case QMI_ERR_OP_DEVICE_UNSUPPORTED_V01:
            res = LteDirectDiscovery_Error_E_NOT_SUPPORTED;
            break;
          default:
            break;
        }
        break;
      default:
        break;
    }
    QCRIL_LOG_INFO("qmi error 0x%x mapped to lted error %d",
            qmi_service_response->result, res);
  }

  QCRIL_LOG_INFO("res = %d", res);
  return res;
} //qcril_qmi_lte_direct_disc_map_qmi_error_to_lted_error

//===========================================================================
// qcril_qmi_lte_direct_disc_map_qmi_result_type_to_to_ril
//===========================================================================
boolean qcril_qmi_lte_direct_disc_map_qmi_result_type_to_to_ril
(
 qmi_lte_disc_result_type_enum_v01 qmi_result,
 LteDirectDiscovery_Result        *result
)
{
  boolean ret = FALSE;

  if (result)
  {
    ret = TRUE;
    switch(qmi_result)
    {
      case QMI_LTE_DISC_SUCCESS_V01:
        *result = LteDirectDiscovery_Result_SUCCESS;
        break;
      case QMI_LTE_DISC_IN_PROGRESS_V01:
        *result = LteDirectDiscovery_Result_IN_PROGRESS;
        break;
      case QMI_LTE_DISC_ERR_INVALID_EXPRESSION_SCOPE_V01:
        *result = LteDirectDiscovery_Result_INVALID_EXPRESSION_SCOPE;
        break;
      case QMI_LTE_DISC_ERR_UNKNOWN_EXPRESSION_V01:
        *result = LteDirectDiscovery_Result_UNKNOWN_EXPRESSION;
        break;
      case QMI_LTE_DISC_ERR_INVALID_DISCOVERY_TYPE_V01:
        *result = LteDirectDiscovery_Result_INVALID_DISCOVERY_TYPE;
        break;
      case QMI_LTE_DISC_ERR_SERVICE_UNAVAILABLE_V01:
        *result = LteDirectDiscovery_Result_SERVICE_NOT_AVAILABLE;
        break;
      case QMI_LTE_DISC_ERR_APP_AUTH_FAILURE_V01:
        *result = LteDirectDiscovery_Result_APP_AUTH_FAILURE;
        break;
      case QMI_LTE_DISC_ERR_FEATURE_NOT_SUPPORTED_V01:
        *result = LteDirectDiscovery_Result_NOT_SUPPORTED;
        break;
      case QMI_LTE_DISC_FAILURE_V01:
      case QMI_LTE_DISC_ERR_UNKNOWN_V01:
      case QMI_LTE_DISC_ERR_NETWORK_TRANS_FAILURE_V01:
      case QMI_LTE_DISC_ERR_MALFORMED_PC3_MSG_V01:
      case QMI_LTE_DISC_ERR_INVALID_TX_ID_V01:
      case QMI_LTE_DISC_ERR_VALIDITY_TIME_EXPIRED_V01:
      default:
        *result = LteDirectDiscovery_Result_GENERIC_FAILURE;
        break;
    }
    QCRIL_LOG_INFO("qmi lte result %d mapped to LteDirectDiscovery result %d",
            qmi_result, *result);
  }
  QCRIL_LOG_INFO("ret = %d", ret);
  return ret;
} //qcril_qmi_lte_direct_disc_map_qmi_result_type_to_to_ril

//===========================================================================
// qcril_qmi_lte_direct_disc_map_event_to_request
//===========================================================================
LteDirectDiscovery_MsgId qcril_qmi_lte_direct_disc_map_event_to_request
(
 qcril_evt_e_type event
)
{
  std::map<qcril_evt_e_type, LteDirectDiscovery_MsgId>::iterator it;
  it = qcril_qmi_lte_direct_disc_event_map.find(event);
  if (it != qcril_qmi_lte_direct_disc_event_map.end())
      return it->second;
  else
      return LteDirectDiscovery_MsgId_REQUEST_BASE;
}

//===========================================================================
// qcril_qmi_lte_direct_disc_map_ril_discovery_type_to_qmi
//===========================================================================
boolean qcril_qmi_lte_direct_disc_map_ril_discovery_type_to_qmi
(
 LteDirectDiscovery_DiscoveryType discovery_type,
 qmi_lte_discovery_type_enum_v01 *qmi_discovery_type
)
{
  boolean ret = FALSE;

  if (qmi_discovery_type)
  {
    ret = TRUE;
    switch(discovery_type)
    {
      case LteDirectDiscovery_DiscoveryType_OPEN:
        *qmi_discovery_type = QMI_LTE_DISC_DISCOVERY_OPEN_V01;
        break;
      case LteDirectDiscovery_DiscoveryType_RESTRICTED:
        *qmi_discovery_type = QMI_LTE_DISC_DISCOVERY_RESTRICTED_V01;
        break;
      default:
        ret = FALSE;
        break;
    }
    QCRIL_LOG_INFO("LteDirectDiscovery discovery type: %d mapped to qmi discovery type: %d",
            discovery_type, *qmi_discovery_type);
  }

  QCRIL_LOG_INFO("ret = %d", ret);
  return ret;
} //qcril_qmi_lte_direct_disc_map_ril_discovery_type_to_qmi

//===========================================================================
// qcril_qmi_lte_direct_disc_map_ril_category_to_qmi
//===========================================================================
boolean qcril_qmi_lte_direct_disc_map_ril_category_to_qmi
(
 uint32_t ril_category,
 qmi_lte_disc_category_type_enum_v01 *qmi_category
)
{
    boolean ret = FALSE;

    if (qmi_category)
    {
        ret = TRUE;
        switch(ril_category)
        {
        case 0:
            *qmi_category = QMI_LTE_DISC_CATEGORY_HIGH_V01;
            break;
        case 1:
            *qmi_category = QMI_LTE_DISC_CATEGORY_MEDIUM_V01;
            break;
        case 2:
            *qmi_category = QMI_LTE_DISC_CATEGORY_LOW_V01;
            break;
        case 3:
            *qmi_category = QMI_LTE_DISC_CATEGORY_VERY_LOW_V01;
            break;
        case 4:
            *qmi_category = QMI_LTE_DISC_CATEGORY_INVALID_V01;
            break;
        default:
            ret = FALSE;
            break;
        }
    }

    QCRIL_LOG_INFO("ret = %d", ret);
    if (ret)
    {
        QCRIL_LOG_INFO("SUCCESS: ril_category = %d is mapped to qmi_category %d",
                ril_category, *qmi_category);
    }
    return ret;
} //qcril_qmi_lte_direct_disc_map_ril_category_to_qmi

//===========================================================================
// qcril_qmi_lte_direct_disc_map_qmi_category_to_ril
//===========================================================================
boolean qcril_qmi_lte_direct_disc_map_qmi_category_to_ril
(
 qmi_lte_disc_category_type_enum_v01 qmi_category,
 uint32_t *ril_category
)
{
    boolean ret = FALSE;

    if (ril_category)
    {
        ret = TRUE;
        switch(qmi_category)
        {
        case QMI_LTE_DISC_CATEGORY_HIGH_V01:
            *ril_category = 0;
            break;
        case QMI_LTE_DISC_CATEGORY_MEDIUM_V01:
            *ril_category = 1;
            break;
        case QMI_LTE_DISC_CATEGORY_LOW_V01:
            *ril_category = 2;
            break;
        case QMI_LTE_DISC_CATEGORY_VERY_LOW_V01:
            *ril_category = 3;
            break;
        case QMI_LTE_DISC_CATEGORY_INVALID_V01:
            *ril_category = 4;
            break;
        default:
            ret = FALSE;
            break;
        }
    }
    QCRIL_LOG_INFO("ret = %d", ret);
    if (ret)
    {
        QCRIL_LOG_INFO("SUCCESS: qmi_category = %d is mapped to ril_category %d",
                qmi_category, *ril_category);
    }
    return ret;
} //qcril_qmi_lte_direct_disc_map_qmi_category_to_ril

//===========================================================================
// qcril_qmi_lte_direct_disc_map_ril_range_to_qmi
//===========================================================================
boolean qcril_qmi_lte_direct_disc_map_ril_range_to_qmi
(
 uint32_t ril_range,
 qmi_lte_disc_announcing_policy_range_enum_v01 *qmi_range
)
{
    boolean ret = FALSE;

    if (qmi_range)
    {
        ret = TRUE;
        switch(ril_range)
        {
        case 0:
            *qmi_range = QMI_LTE_DISC_ANNOUNCING_POLICY_INVALID_V01;
            break;
        case 1:
            *qmi_range = QMI_LTE_DISC_ANNOUNCING_POLICY_SHORT_V01;
            break;
        case 2:
            *qmi_range = QMI_LTE_DISC_ANNOUNCING_POLICY_MEDIUM_V01;
            break;
        case 3:
            *qmi_range = QMI_LTE_DISC_ANNOUNCING_POLICY_LONG_V01;
            break;
        case 4:
            *qmi_range = QMI_LTE_DISC_ANNOUNCING_POLICY_RESERVED_V01;
            break;
        default:
            ret = FALSE;
            break;
        }
    }
    QCRIL_LOG_INFO("ret = %d", ret);
    if (ret)
    {
        QCRIL_LOG_INFO("SUCCESS: ril_range = %d is mapped to qmi_range %d",
                ril_range, *qmi_range);
    }
    return ret;
} //qcril_qmi_lte_direct_disc_map_ril_range_to_qmi

//===========================================================================
// qcril_qmi_lte_direct_disc_map_qmi_range_to_ril
//===========================================================================
boolean qcril_qmi_lte_direct_disc_map_qmi_range_to_ril
(
 qmi_lte_disc_announcing_policy_range_enum_v01 qmi_range,
 uint32_t *ril_range
)
{
    boolean ret = FALSE;

    if (ril_range)
    {
        ret = TRUE;
        switch(qmi_range)
        {
        case QMI_LTE_DISC_ANNOUNCING_POLICY_INVALID_V01:
            *ril_range = 0;
            break;
        case QMI_LTE_DISC_ANNOUNCING_POLICY_SHORT_V01:
            *ril_range = 1;
            break;
        case QMI_LTE_DISC_ANNOUNCING_POLICY_MEDIUM_V01:
            *ril_range = 2;
            break;
        case QMI_LTE_DISC_ANNOUNCING_POLICY_LONG_V01:
            *ril_range = 3;
            break;
        case QMI_LTE_DISC_ANNOUNCING_POLICY_RESERVED_V01:
            *ril_range = 4;
            break;
        default:
            ret = FALSE;
            break;
        }
    }
    QCRIL_LOG_INFO("ret = %d", ret);
    if (ret)
    {
        QCRIL_LOG_INFO("SUCCESS: qmi_range = %d is mapped to ril_range %d",
                qmi_range, *ril_range);
    }
    return ret;
} //qcril_qmi_lte_direct_disc_map_qmi_range_to_ril

//===========================================================================
// qcril_qmi_lte_direct_disc_map_ril_plmn_to_qmi
//
//  QMI PLMN is expected to be in the below format:
//  From lte_v01.h interface file:
//
//  A PLMN ID is defined to be the combination of a Mobile Country Code (MCC) and
//  Mobile Network Code (MNC). A PLMN ID is stored in three octets as below:
//
//
//                                        Bit
//                 |  8  |  7  |  6  |  5  |  4  |  3  |  2  |  1  |
//                 |===============================================|
//         octet 1 |      MCC Digit 2      |      MCC Digit 1      |
//                 |-----------------------------------------------|
//         octet 2 |      MNC Digit 3      |      MCC Digit 3      |
//                 |-----------------------------------------------|
//         octet 3 |      MNC Digit 2      |      MNC Digit 1      |
//                 |===============================================|
//
//===========================================================================
boolean qcril_qmi_lte_direct_disc_map_ril_plmn_to_qmi
(
 com_qualcomm_qti_radioconfiginterface_Plmn *ril_plmn,
 qmi_lte_disc_plmn_id_s_type_v01            *qmi_plmn
)
{
  boolean ret = FALSE;
  qmi_lte_disc_plmn_id_s_type_v01 result_qmi_plmn;

  if (ril_plmn && qmi_plmn)
  {
    QCRIL_LOG_ERROR("ril_plmn: mcc = %s, mnc = %s", ril_plmn->mcc, ril_plmn->mnc);

    memset(&result_qmi_plmn.plmn_id,
        0xFF,
        sizeof(result_qmi_plmn.plmn_id));

    int mcc_len = strlen(ril_plmn->mcc);
    int mnc_len = strlen(ril_plmn->mnc);
    if (mcc_len == 3)
    {
      uint8_t d1 = ril_plmn->mcc[0] - '0';
      uint8_t d2 = ril_plmn->mcc[1] - '0';
      // Validate digit beteen 0 and 9
      if ((d1 <= 9) && (d2 <= 9))
      {
        result_qmi_plmn.plmn_id[0] = (((0x0F & d2) << 4) | (0x0F & d1));
      }

      d1 = ril_plmn->mcc[2] - '0';
      d2 = 0xFF;
      // MNC Digit 3
      if (mnc_len == 3)
      {
        d2 = ril_plmn->mnc[2] - '0';
      }
      // Validate digit beteen 0 and 9 (d2 can be 0xFF)
      if ((d1 <= 9) &&
          (d2 == 0xFF || d2 <= 9))
      {
        result_qmi_plmn.plmn_id[1] = (((0x0F & d2) << 4) | (0x0F & d1));
      }

      if (mnc_len >= 2)
      {
        d1 = ril_plmn->mnc[0] - '0';
        d2 = ril_plmn->mnc[1] - '0';
      }
      // Validate digit beteen 0 - 9
      if ((d1 <= 9) && (d2 <= 9))
      {
        result_qmi_plmn.plmn_id[2] = (((0x0F & d2) << 4) | (0x0F & d1));
      }
      ret = TRUE;
    }
  }
  else
  {
    QCRIL_LOG_ERROR("Invalid parameters");
  }

  if (ret == TRUE)
  {
    memcpy(qmi_plmn, &result_qmi_plmn, sizeof(result_qmi_plmn));
    QCRIL_LOG_ERROR("qmi_plmn: 0x%x, 0x%x, 0x%x",
        qmi_plmn->plmn_id[0], qmi_plmn->plmn_id[1], qmi_plmn->plmn_id[2]);
  }

  return ret;
} //qcril_qmi_lte_direct_disc_map_ril_plmn_to_qmi

//===========================================================================
// qcril_qmi_lte_direct_disc_map_qmi_plmn_to_ril
//===========================================================================
boolean qcril_qmi_lte_direct_disc_map_qmi_plmn_to_ril
(
 qmi_lte_disc_plmn_id_s_type_v01            *qmi_plmn,
 com_qualcomm_qti_radioconfiginterface_Plmn *ril_plmn
)
{
  boolean ret = FALSE;
  char mcc[4];
  char mnc[4];

  if (ril_plmn && qmi_plmn)
  {
    QCRIL_LOG_ERROR("qmi_plmn: 0x%x, 0x%x, 0x%x",
        qmi_plmn->plmn_id[0], qmi_plmn->plmn_id[1], qmi_plmn->plmn_id[2]);

    memset(mcc, 0x00, sizeof(mcc));
    memset(mnc, 0x00, sizeof(mnc));

    // plmn_id[0] is |      MCC Digit 2      |      MCC Digit 1      |
    uint8_t d = (qmi_plmn->plmn_id[0] & 0x0F);
    mcc[0] = d + '0';

    d = ((qmi_plmn->plmn_id[0] >> 4) & 0x0F);
    mcc[1] = d + '0';

    // plmn_id[1] is |      MNC Digit 3      |      MCC Digit 3      |
    d = (qmi_plmn->plmn_id[1] & 0x0F);
    mcc[2] = d + '0';

    d = ((qmi_plmn->plmn_id[1] >> 4) & 0x0F);
    if (d != 0xF) {
      mnc[2] = d + '0';
    }

    // plmn_id[2] |      MNC Digit 2      |      MNC Digit 1      |
    d = (qmi_plmn->plmn_id[2] & 0x0F);
    mnc[0] = d + '0';

    d = ((qmi_plmn->plmn_id[2] >> 4) & 0x0F);
    mnc[1] = d + '0';

    ret = TRUE;
  }
  else
  {
    QCRIL_LOG_ERROR("Invalid parameters");
  }

  if (ret == TRUE)
  {
    ril_plmn->mcc = qmi_ril_util_str_clone(mcc);
    ril_plmn->mnc = qmi_ril_util_str_clone(mnc);

    QCRIL_LOG_ERROR("mcc = %s, mnc = %s", ril_plmn->mcc, ril_plmn->mnc);
  }

  return ret;
} //qcril_qmi_lte_direct_disc_map_qmi_plmn_to_ril
