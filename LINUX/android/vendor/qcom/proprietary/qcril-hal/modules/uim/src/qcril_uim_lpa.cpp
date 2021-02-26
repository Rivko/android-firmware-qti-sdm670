/*===========================================================================

Copyright (c) 2016 - 2017 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.

===========================================================================*/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#include "modules/uim/qcril_uim.h"
#include "modules/uim/qcril_uim_srvc.h"
#include "modules/uim/qcril_uim_lpa.h"
#include "modules/uim/UimLpaIndicationMsg.h"
#include "qcril_uim_util.h"

#define TAG "qcril_uim_lpa"

/*===========================================================================
                        INTERNAL DEFINITIONS AND TYPES
===========================================================================*/
typedef struct
{
  qcril_uim_lpa_user_req_event_type  event;
  std::shared_ptr<UimLpaReqMessage>  req_ptr;
  uint8                              iccid[QCRIL_UIM_ICCID_LEN];
} qcril_uim_lpa_user_req_orig_type;

/*---------------------------------------------------------------------------
  global variables
---------------------------------------------------------------------------*/
static uint8_t  qcril_uim_lpa_profileId_iccId[QMI_UIM_PROFILES_MAX][QCRIL_UIM_ICCID_LEN] = {{0xFF}};
static boolean  qcril_uim_lpa_profiles_cached = FALSE;

/*===========================================================================

                               INTERNAL FUNCTIONS

===========================================================================*/
/*=========================================================================

  FUNCTION:  qcril_uim_lpa_convert_slot_id_to_slot_type

===========================================================================*/
static boolean qcril_uim_lpa_convert_slot_id_to_slot_type
(
  uint8                 slot_id,
  qmi_uim_slot_type   * uim_slot_ptr
)
{
  if (uim_slot_ptr == NULL)
  {
    return FALSE;
  }

  switch (slot_id)
  {
     case 0:
       *uim_slot_ptr = QMI_UIM_SLOT_1;
       break;

     case 1:
       *uim_slot_ptr = QMI_UIM_SLOT_2;
       break;

    case 2:
      *uim_slot_ptr = QMI_UIM_SLOT_3;
      break;

    default:
      QCRIL_LOG_ERROR( "Invalid _id returned: 0x%x\n", slot_id);
      return FALSE;
  }

  return TRUE;
} /* qcril_uim_lpa_convert_slot_id_to_slot_type */


/*=========================================================================

  FUNCTION:  qcril_uim_lpa_byte_to_iccid

===========================================================================*/
static uint8_t qcril_uim_lpa_byte_to_iccid
(
  uint8_t       * iccid_ptr,
  uint8_t         iccid_len,
  const uint8_t * iccid_byte_ptr,
  uint8_t         byte_len
)
{
  uint8_t i = 0;
  uint8_t j = 0;
  int8_t ch = -1;

  if (iccid_ptr == NULL ||
      iccid_byte_ptr == NULL ||
      iccid_len == 0 ||
      iccid_len > QCRIL_UIM_ICCID_LEN ||
      byte_len == 0 ||
      byte_len > 2 * QCRIL_UIM_ICCID_LEN)
  {
    QCRIL_LOG_ERROR("iccid ptr or iccid_str_ptr parameter is NULL");
    return 0;
  }

  for ( i = 0; i < byte_len && j < iccid_len; j++ )
  {
    /* copy first bcd digit */
    ch = iccid_byte_ptr[i] - '0';
    if (ch < 0 || ch > 9)
    {
      return 0;
    }
    iccid_ptr[j] = ch;
    i++;

    /* copy second bcd digit */
    if ( i < byte_len )
    {
      ch = iccid_byte_ptr[i] - '0';
      if (ch < 0 || ch > 9)
      {
        return 0;
      }
      ch = ch << 4;
      iccid_ptr[j] |= ch;
      i++;
    }
    else
    {
      iccid_ptr[j] |= 0xF0;
    }
  }
  return j;
} /* qcril_uim_lpa_byte_to_iccid */


/*=========================================================================

  FUNCTION:  qcril_uim_lpa_get_profile_id_from_iccid

===========================================================================*/
static int8_t qcril_uim_lpa_get_profile_id_from_iccid
(
  uint8_t *iccid_ptr
)
{
  uint8 i = 0;

  if (iccid_ptr == NULL)
  {
    return -1;
  }

  for (i = 0; i < QMI_UIM_PROFILES_MAX; i++)
  {
    if (memcmp(iccid_ptr, qcril_uim_lpa_profileId_iccId[i], QCRIL_UIM_ICCID_LEN) == 0)
    {
      /* Profile Id starts from 1, index starts from 0, index + 1 is profile ID*/
      return (i+1);
    }
  }

  return -1;
} /* qcril_uim_lpa_get_profile_id_from_iccid */


/*===========================================================================

  FUNCTION:  qcril_uim_lpa_add_profile

===========================================================================*/
static bool qcril_uim_lpa_add_profile
(
  void             * msg_ptr,
  const char       * activationCode_ptr,
  const char       * confirmationCode_ptr
)
{
  int                                   qmi_err_code           = 0;
  qmi_uim_add_profile_req_type          add_profile_req;
  uint8                                 slot                   = QMI_UIM_MAX_CARD_COUNT;

  memset(&add_profile_req, 0x00, sizeof(add_profile_req));

  if (activationCode_ptr == NULL ||
      strlen(activationCode_ptr) == 0 ||
      strlen(activationCode_ptr) >= QMI_UIM_EUICC_DOWNLOAD_CODE_MAX)
  {
    return FALSE;
  }

  slot = qcril_uim_instance_id_to_slot(qmi_ril_get_process_instance_id());
  if (slot >= QMI_UIM_MAX_CARD_COUNT)
  {
    return FALSE;
  }

  if (!qcril_uim_lpa_convert_slot_id_to_slot_type(slot, &add_profile_req.slot))
  {
    return FALSE;
  }

  add_profile_req.activation_code_ptr = (char *)activationCode_ptr;
  add_profile_req.confirmation_code_ptr = (char *)confirmationCode_ptr;

  qmi_err_code = qcril_uim_send_qmi_async_msg(QCRIL_UIM_REQUEST_ADD_PROFILE,
                                              &add_profile_req,
                                              msg_ptr);

  if (qmi_err_code != 0)
  {
    return FALSE;
  }

   return TRUE;
} /* qcril_uim_lpa_add_profile */


/*===========================================================================

  FUNCTION:  qcril_uim_lpa_get_profile

===========================================================================*/
static bool qcril_uim_lpa_get_profile
(
  void   * msg_ptr
)
{
  int                                   qmi_err_code           = 0;
  qmi_uim_get_sim_profiles_type         get_profiles_req;
  uint8                                 slot                   = QMI_UIM_MAX_CARD_COUNT;

  memset(&get_profiles_req, 0, sizeof(get_profiles_req));

  slot = qcril_uim_instance_id_to_slot(qmi_ril_get_process_instance_id());
  if (slot >= QMI_UIM_MAX_CARD_COUNT)
  {
    return FALSE;
  }

  if (!qcril_uim_lpa_convert_slot_id_to_slot_type(slot, &get_profiles_req.slot))
  {
    return FALSE;
  }

  qmi_err_code = qcril_uim_send_qmi_async_msg(QCRIL_UIM_REQUEST_GET_SIM_PROFILES,
                                              &get_profiles_req,
                                              msg_ptr);

  if (qmi_err_code != 0)
  {
    return FALSE;
  }

  return TRUE;
} /* qcril_uim_lpa_get_profile */


/*===========================================================================

  FUNCTION:  qcril_uim_lpa_enable_disable_profile

===========================================================================*/
static bool qcril_uim_lpa_enable_disable_profile
(
  void                                * msg_ptr,
  uint8_t                             * iccid_ptr,
  qcril_uim_lpa_user_req_event_type     event_id,
  boolean                               from_get_profile
)
{
  int                                qmi_err_code            = 0;
  qmi_uim_set_sim_profile_req_type   en_dis_profile_req;
  uint8                              slot                    = QMI_UIM_MAX_CARD_COUNT;
  int8_t                             profile_id              = -1;

  memset(&en_dis_profile_req, 0, sizeof(en_dis_profile_req));

  if (iccid_ptr == NULL ||
      iccid_ptr[0] == 0xFF)
  {
    return FALSE;
  }

  slot = qcril_uim_instance_id_to_slot(qmi_ril_get_process_instance_id());
  if (slot >= QMI_UIM_MAX_CARD_COUNT)
  {
    return FALSE;
  }

  if (!qcril_uim_lpa_convert_slot_id_to_slot_type(slot, &en_dis_profile_req.slot))
  {
    return FALSE;
  }

  if (!qcril_uim_lpa_profiles_cached &&
      !from_get_profile)
  {
    if (qcril_uim_lpa_get_profile(msg_ptr))
    {
      return TRUE;
    }
  }

  profile_id = qcril_uim_lpa_get_profile_id_from_iccid(iccid_ptr);

  if (profile_id < 0 || profile_id > QMI_UIM_PROFILES_MAX)
  {
    return FALSE;
  }

  en_dis_profile_req.profile_id = (qmi_uim_profile_id_type)profile_id;

  if (event_id == QCRIL_UIM_LPA_ENABLE_PROFILE)
  {
    en_dis_profile_req.enable_profile = TRUE;
  }
  else
  {
    en_dis_profile_req.enable_profile = FALSE;
  }

  qmi_err_code = qcril_uim_send_qmi_async_msg(QCRIL_UIM_REQUEST_SET_SIM_PROFILE,
                                              &en_dis_profile_req,
                                              msg_ptr);

  if (qmi_err_code != 0)
  {
    return FALSE;
  }

  return TRUE;
} /* qcril_uim_lpa_enable_disable_profile */


/*===========================================================================

  FUNCTION:  qcril_uim_lpa_delete_profile

===========================================================================*/
static bool qcril_uim_lpa_delete_profile
(
  void                 * msg_ptr,
  uint8_t              * iccid_ptr,
  boolean                from_get_profile
)
{
  int                                qmi_err_code         = 0;
  qmi_uim_delete_profile_req_type    del_profile_req;
  uint8                              slot                 = QMI_UIM_MAX_CARD_COUNT;
  int8_t                             profile_id           = -1;

  memset(&del_profile_req, 0, sizeof(del_profile_req));

  if (iccid_ptr == NULL ||
      iccid_ptr[0] == 0xFF)
  {
    return FALSE;
  }

  slot = qcril_uim_instance_id_to_slot(qmi_ril_get_process_instance_id());
  if (slot >= QMI_UIM_MAX_CARD_COUNT)
  {
    return FALSE;
  }

  if (!qcril_uim_lpa_convert_slot_id_to_slot_type(slot, &del_profile_req.slot))
  {
    return FALSE;
  }

  if (!qcril_uim_lpa_profiles_cached &&
      !from_get_profile)
  {
    if (qcril_uim_lpa_get_profile(msg_ptr))
    {
      return TRUE;
    }
  }

  profile_id = qcril_uim_lpa_get_profile_id_from_iccid(iccid_ptr);

  if (profile_id < 0 || profile_id > QMI_UIM_PROFILES_MAX)
  {
    return FALSE;
  }

  del_profile_req.profile_id = (qmi_uim_profile_id_type)profile_id;

  qmi_err_code = qcril_uim_send_qmi_async_msg(QCRIL_UIM_REQUEST_DELETE_PROFILE,
                                              &del_profile_req,
                                              msg_ptr);

  if (qmi_err_code != 0)
  {
    return FALSE;
  }

  return TRUE;
} /* qcril_uim_lpa_delete_profile */


/*===========================================================================

  FUNCTION:  qcril_uim_lpa_memory_reset

===========================================================================*/
static bool qcril_uim_lpa_memory_reset
(
  void                 * msg_ptr,
  int32                  resetMask
)
{
  int                                   qmi_err_code           = 0;
  qmi_uim_euicc_memory_reset_req_type   memory_reset_req;
  uint8                                 slot                   = QMI_UIM_MAX_CARD_COUNT;

  memset(&memory_reset_req, 0, sizeof(memory_reset_req));

  slot = qcril_uim_instance_id_to_slot(qmi_ril_get_process_instance_id());
  if (slot >= QMI_UIM_MAX_CARD_COUNT)
  {
    return FALSE;
  }

  if (!qcril_uim_lpa_convert_slot_id_to_slot_type(slot, &memory_reset_req.slot))
  {
    return FALSE;
  }

  /* Set the reset mask */
  if (resetMask != 0)
  {
    if (resetMask & QCRIL_UIM_LPA_EUICC_RESET_TEST_PROFILES)
    {
      memory_reset_req.reset_option |= QMI_UIM_RESET_TEST_PROFILES;
    }

    if (resetMask & QCRIL_UIM_LPA_EUICC_RESET_OPERATIONAL_PROFILES)
    {
      memory_reset_req.reset_option |= QMI_UIM_RESET_OPERATIONAL_PROFILES;
    }

    if (resetMask & QCRIL_UIM_LPA_EUICC_RESET_TO_DEFAULT_SMDP_ADDRESS)
    {
      memory_reset_req.reset_option |= QMI_UIM_RESET_TO_DEFAULT_SMDP_ADDRESS;
    }
  }

  qmi_err_code = qcril_uim_send_qmi_async_msg(QCRIL_UIM_REQUEST_EUICC_MEMORY_RESET,
                                              &memory_reset_req,
                                              msg_ptr);

  if (qmi_err_code != 0)
  {
    return FALSE;
  }

  return TRUE;
} /* qcril_uim_lpa_memory_reset */


/*===========================================================================

  FUNCTION:  qcril_uim_lpa_update_nickname

===========================================================================*/
static bool qcril_uim_lpa_update_nickname
(
  void                 * msg_ptr,
  uint8_t              * iccid_ptr,
  const char           * nick_name_ptr
)
{
  int                                        qmi_err_code       = 0;
  qmi_uim_update_profile_nickname_req_type   nick_name_req;
  uint8                                      slot               = QMI_UIM_MAX_CARD_COUNT;
  int8_t                                     profile_id         = -1;

  memset(&nick_name_req, 0, sizeof(nick_name_req));

  if (iccid_ptr == NULL)
  {
    return FALSE;
  }

  slot = qcril_uim_instance_id_to_slot(qmi_ril_get_process_instance_id());
  if (slot >= QMI_UIM_MAX_CARD_COUNT)
  {
    return FALSE;
  }

  if (!qcril_uim_lpa_convert_slot_id_to_slot_type(slot, &nick_name_req.slot))
  {
    return FALSE;
  }

  profile_id = qcril_uim_lpa_get_profile_id_from_iccid(iccid_ptr);

  if (profile_id < 0 || profile_id > QMI_UIM_PROFILES_MAX)
  {
    return FALSE;
  }

  nick_name_req.profile_id = (qmi_uim_profile_id_type)profile_id;

  if (nick_name_ptr == NULL ||
      strlen(nick_name_ptr) > QMI_UIM_EUICC_NICKNAME_MAX)
  {
    return FALSE;
  }

  nick_name_req.nickname = (char *)nick_name_ptr;


  qmi_err_code = qcril_uim_send_qmi_async_msg(QCRIL_UIM_REQUEST_UPDATE_PROFILE_NICKNAME,
                                              &nick_name_req,
                                              msg_ptr);

  if (qmi_err_code != 0)
  {
    return FALSE;
  }

  return TRUE;
} /* qcril_uim_lpa_update_nickname */


/*===========================================================================

  FUNCTION:  qcril_uim_lpa_user_consent

===========================================================================*/
static bool qcril_uim_lpa_user_consent
(
  void                 * msg_ptr,
  boolean                user_ok
)
{
  int                                     qmi_err_code           = 0;
  qmi_uim_profile_user_consent_req_type   user_consent_req;
  uint8                                   slot                   = QMI_UIM_MAX_CARD_COUNT;

  memset(&user_consent_req, 0, sizeof(user_consent_req));

  slot = qcril_uim_instance_id_to_slot(qmi_ril_get_process_instance_id());
  if (slot >= QMI_UIM_MAX_CARD_COUNT)
  {
    return FALSE;
  }

  if (!qcril_uim_lpa_convert_slot_id_to_slot_type(slot, &user_consent_req.slot))
  {
    return FALSE;
  }

  user_consent_req.user_ok = (uint8_t)user_ok;

  qmi_err_code = qcril_uim_send_qmi_async_msg(QCRIL_UIM_REQUEST_USER_CONSENT,
                                              &user_consent_req,
                                              msg_ptr);

  if (qmi_err_code != 0)
  {
    return FALSE;
  }

  return TRUE;
} /* qcril_uim_lpa_user_consent */


/*===========================================================================

  FUNCTION:  qcril_uim_lpa_get_eid

===========================================================================*/
static bool qcril_uim_lpa_get_eid
(
  void     * msg_ptr
)
{
  int                                   qmi_err_code = 0;
  qmi_uim_get_eid_req_type              eid_req;
  uint8                                 slot         = QMI_UIM_MAX_CARD_COUNT;

  memset(&eid_req, 0, sizeof(eid_req));

  slot = qcril_uim_instance_id_to_slot(qmi_ril_get_process_instance_id());
  if (slot >= QMI_UIM_MAX_CARD_COUNT)
  {
    return FALSE;
  }

  if (!qcril_uim_lpa_convert_slot_id_to_slot_type(slot, &eid_req.slot))
  {
    return FALSE;
  }

  qmi_err_code = qcril_uim_send_qmi_async_msg(QCRIL_UIM_REQUEST_GET_EID,
                                              &eid_req,
                                              msg_ptr);

  if (qmi_err_code != 0)
  {
    return FALSE;
  }

  return TRUE;
} /* qcril_uim_lpa_get_eid */


/*===========================================================================

  FUNCTION:  qcril_lpa_uim_get_profile_info

===========================================================================*/
static boolean qcril_lpa_uim_get_profile_info
(
  qcril_uim_lpa_profile_info_type  * profile_info_ptr,
  qmi_uim_slot_type                  slot_id,
  qmi_uim_profile_id_type            profile_id
)
{
  qmi_uim_get_profile_info_req_type    profile_info_req;
  qmi_uim_rsp_data_type              * profile_info_resp_ptr = NULL;
  int                                  rc                    = -1;

  memset(&profile_info_req, 0, sizeof(qmi_uim_get_profile_info_req_type));

  profile_info_req.slot       = slot_id;
  profile_info_req.profile_id = profile_id;

  if (profile_info_ptr == NULL ||
      profile_id == 0 ||
      profile_id > QMI_UIM_PROFILE_ID_8 ||
      profile_id > QMI_UIM_PROFILES_MAX)
  {
    return FALSE;
  }

  profile_info_resp_ptr = new qmi_uim_rsp_data_type;

  if (profile_info_resp_ptr == NULL)
  {
    return FALSE;
  }

  memset(profile_info_resp_ptr, 0x00, sizeof(qmi_uim_rsp_data_type));
  memset(profile_info_ptr, 0x00, sizeof(qcril_uim_lpa_profile_info_type));

  rc = qcril_uim_send_qmi_sync_msg(QCRIL_UIM_REQUEST_GET_PROFILE_INFO,
                                   &profile_info_req,
                                   profile_info_resp_ptr);

  if ((rc != 0) ||
      (profile_info_resp_ptr->sys_err_code != 0) ||
      (profile_info_resp_ptr->qmi_err_code) != 0)
  {
    QCRIL_UIM_FREE_IF_NOT_NULL(profile_info_resp_ptr);
    return FALSE;
  }

  /* Copy state */
  switch (profile_info_resp_ptr->rsp_data.profile_info_rsp.state)
  {
    case QMI_UIM_PROFILE_STATE_INACTIVE:
      profile_info_ptr->state = QCRIL_UIM_LPA_PROFILE_STATE_INACTIVE;
      break;
    case QMI_UIM_PROFILE_STATE_ACTIVE:
      profile_info_ptr->state = QCRIL_UIM_LPA_PROFILE_STATE_ACTIVE;
      break;
    default:
      profile_info_ptr->state = QCRIL_UIM_LPA_PROFILE_STATE_UNKNOWN;
      break;
  }

  /* Copy icon */
  switch (profile_info_resp_ptr->rsp_data.profile_info_rsp.icon_type)
  {
    case QMI_UIM_PROFILE_ICON_TYPE_JPEG:
      profile_info_ptr->iconType = QCRIL_UIM_LPA_ICON_TYPE_JPEG;
      break;
    case QMI_UIM_PROFILE_ICON_TYPE_PNG:
      profile_info_ptr->iconType = QCRIL_UIM_LPA_ICON_TYPE_PNG;
      break;
    default:
      profile_info_ptr->iconType = QCRIL_UIM_LPA_ICON_TYPE_UNKNOWN;
      break;
  }

   /* Copy icon data */

  if (profile_info_resp_ptr->rsp_data.profile_info_rsp.icon_len != 0 &&
      profile_info_resp_ptr->rsp_data.profile_info_rsp.icon_data_ptr != nullptr)
  {
    profile_info_ptr->icon = new uint8_t[profile_info_resp_ptr->rsp_data.profile_info_rsp.icon_len]{};
    if (profile_info_ptr->icon != nullptr)
    {
      memcpy(profile_info_ptr->icon,
             profile_info_resp_ptr->rsp_data.profile_info_rsp.icon_data_ptr,
             profile_info_resp_ptr->rsp_data.profile_info_rsp.icon_len);

      profile_info_ptr->icon_len = profile_info_resp_ptr->rsp_data.profile_info_rsp.icon_len;
    }
  }

  /* Copy ICCID */
  if (profile_info_resp_ptr->rsp_data.profile_info_rsp.iccid_len != 0 &&
      profile_info_resp_ptr->rsp_data.profile_info_rsp.iccid_ptr != nullptr)
  {
    profile_info_ptr->iccid = new uint8_t[profile_info_resp_ptr->rsp_data.profile_info_rsp.iccid_len]{};

    if (profile_info_ptr->iccid != nullptr)
    {
      memcpy(profile_info_ptr->iccid,
             profile_info_resp_ptr->rsp_data.profile_info_rsp.iccid_ptr,
             profile_info_resp_ptr->rsp_data.profile_info_rsp.iccid_len);

      profile_info_ptr->iccid_len = profile_info_resp_ptr->rsp_data.profile_info_rsp.iccid_len;

      /* Cache the profile ID and ICCID */
      memcpy(qcril_uim_lpa_profileId_iccId[profile_id - 1],
             profile_info_ptr->iccid,
             profile_info_ptr->iccid_len);
    }
  }

  /* Copy profile name */
  if (profile_info_resp_ptr->rsp_data.profile_info_rsp.name != nullptr)
  {
    uint32_t str_len = strlen(profile_info_resp_ptr->rsp_data.profile_info_rsp.name);

    profile_info_ptr->profileName = new char[str_len + 1]{};
    if (profile_info_ptr->profileName != nullptr)
    {
      memcpy(profile_info_ptr->profileName,
             profile_info_resp_ptr->rsp_data.profile_info_rsp.name,
             str_len);
    }
  }

  /* Copy nick name */
  if (profile_info_resp_ptr->rsp_data.profile_info_rsp.nickname != nullptr)
  {
    uint32_t str_len = strlen(profile_info_resp_ptr->rsp_data.profile_info_rsp.nickname);

    profile_info_ptr->nickName = new char[str_len + 1]{};
    if (profile_info_ptr->nickName != nullptr)
    {
      memcpy(profile_info_ptr->nickName,
             profile_info_resp_ptr->rsp_data.profile_info_rsp.nickname,
             str_len);
    }
  }

  /* Copy SPN name */
  if (profile_info_resp_ptr->rsp_data.profile_info_rsp.spn != nullptr)
  {
    uint32_t str_len = strlen(profile_info_resp_ptr->rsp_data.profile_info_rsp.spn);

    profile_info_ptr->spName = new char[str_len + 1]{};
    if (profile_info_ptr->spName != nullptr)
    {
      memcpy(profile_info_ptr->spName,
             profile_info_resp_ptr->rsp_data.profile_info_rsp.spn,
             str_len);
    }
  }

  /* Copy profile class */
  switch (profile_info_resp_ptr->rsp_data.profile_info_rsp.pclass)
  {
    case QMI_UIM_PROFILE_CLASS_TEST:
      profile_info_ptr->profileClass = QCRIL_UIM_LPA_PROFILE_CLASS_TEST;
      break;
    case QMI_UIM_PROFILE_CLASS_PROVISIONING:
      profile_info_ptr->profileClass = QCRIL_UIM_LPA_PROFILE_CLASS_PROVISIONING;
      break;
    case QMI_UIM_PROFILE_CLASS_OPERATIONAL:
      profile_info_ptr->profileClass = QCRIL_UIM_LPA_PROFILE_CLASS_OPERATIONAL;
      break;
    default:
      break;
  }

  /* Copy PPR info */
  profile_info_ptr->profilePolicy =
    (qcril_uim_lpa_profile_policy_mask)profile_info_resp_ptr->rsp_data.profile_info_rsp.policy_rules;

  delete profile_info_resp_ptr;
  return TRUE;
} /* qcril_lpa_uim_get_profile_info */


/*===========================================================================

  FUNCTION:  qcril_uim_lpa_get_set_server_address

===========================================================================*/
static bool qcril_uim_lpa_get_set_server_address
(
  void                           * msg_ptr,
  const char                     * smdp_address_ptr,
  qcril_uim_lpa_srv_addr_op_type   operation_type
)
{
  int                                             qmi_err_code            = 0;
  qmi_uim_euicc_default_server_address_req_type   server_address_req;
  uint8                                           slot                    = QMI_UIM_MAX_CARD_COUNT;

  memset(&server_address_req, 0, sizeof(qmi_uim_euicc_default_server_address_req_type));

  slot = qcril_uim_instance_id_to_slot(qmi_ril_get_process_instance_id());
  if (slot >= QMI_UIM_MAX_CARD_COUNT)
  {
    return FALSE;
  }

  if (!qcril_uim_lpa_convert_slot_id_to_slot_type(slot, &server_address_req.slot))
  {
    return FALSE;
  }

  switch (operation_type)
  {
    case QCRIL_UIM_LPA_SERVER_ADDRESS_SET_OPERATION:
      if (smdp_address_ptr == NULL ||
          strlen(smdp_address_ptr) > QMI_UIM_FQDN_MAX)
      {
        return FALSE;
      }
      server_address_req.default_smdp_address = (char *)smdp_address_ptr;
      break;

    case QCRIL_UIM_LPA_SERVER_ADDRESS_GET_OPERATION:
      break;

    default:
      return FALSE;
  }

  qmi_err_code = qcril_uim_send_qmi_async_msg(QCRIL_UIM_REQUEST_GET_SET_SERVER_ADDRESS,
                                              &server_address_req,
                                              msg_ptr);

  if (qmi_err_code != 0)
  {
    return FALSE;
  }

  return TRUE;
} /* qcril_uim_lpa_get_set_server_address */


/*===========================================================================

  FUNCTION:  qcril_uim_lpa_get_profile_info_resp

===========================================================================*/
static void qcril_uim_lpa_get_profile_info_resp
(
  qmi_uim_get_profiles_resp_type      * get_profiles_rsp_ptr,
  qcril_uim_lpa_user_resp_type        * lpa_rsp_ptr,
  qcril_uim_lpa_user_req_orig_type    * orig_ptr
)
{
  bool                                 result                  = FALSE;
  uint8_t                              no_of_profiles          = 0;
  uint8_t                              i                       = 0;
  qcril_uim_lpa_profile_info_type   *  profiles_ptr            = NULL;
  uint8                                slot                    = QMI_UIM_MAX_CARD_COUNT;
  qmi_uim_slot_type                    qmi_slot                = QMI_UIM_SLOT_1;
  bool                                 profile_info_result     = TRUE;

  if (lpa_rsp_ptr == NULL)
  {
    QCRIL_LOG_ERROR("NULL lpa_rsp_ptr");
    return;
  }

  lpa_rsp_ptr->result = QCRIL_UIM_LPA_RESULT_FAILURE;

  slot = qcril_uim_instance_id_to_slot(qmi_ril_get_process_instance_id());
  if (slot >= QMI_UIM_MAX_CARD_COUNT)
  {
    return;
  }

  if (!qcril_uim_lpa_convert_slot_id_to_slot_type(slot, &qmi_slot))
  {
    return;
  }

  if (get_profiles_rsp_ptr == NULL)
  {
    QCRIL_LOG_ERROR("NULL get_profiles_rsp_ptr");
    return;
  }

  no_of_profiles = get_profiles_rsp_ptr->profiles_len;

  if (no_of_profiles > QMI_UIM_PROFILES_MAX)
  {
    no_of_profiles = QMI_UIM_PROFILES_MAX;
  }

  /* Protobuff encoder, runs the loop for encoding until it get null pointer.
     Allocate one extra pointer for null pointer termination */
  profiles_ptr = new qcril_uim_lpa_profile_info_type[no_of_profiles];

  if (profiles_ptr == NULL)
  {
    QCRIL_LOG_ERROR("NULL profile ptr");
    return;
  }

  memset(profiles_ptr, 0x00, no_of_profiles * sizeof(qcril_uim_lpa_profile_info_type));

  QCRIL_LOG_DEBUG("No of profile 0x%x", no_of_profiles);

  for (i = 0; i < no_of_profiles && i < QMI_UIM_PROFILES_MAX; i++)
  {
    if (qcril_lpa_uim_get_profile_info(&profiles_ptr[i],
                                       qmi_slot,
                                       get_profiles_rsp_ptr->profiles[i].profile_id) == FALSE)
    {
      profile_info_result = FALSE;
      /* Clean up cached ICCIDs */
      qcril_uim_lpa_cleanup_cached_iccids();
      break;
    }
  }

  if (!profile_info_result && profiles_ptr != nullptr)
  {
    for (i = 0; i < no_of_profiles && i < QMI_UIM_PROFILES_MAX; i++)
    {
      if(profiles_ptr[i].iccid != nullptr)
      {
        delete[] profiles_ptr[i].iccid;
      }
      if(profiles_ptr[i].icon != nullptr)
      {
        delete[] profiles_ptr[i].icon;
      }
      if(profiles_ptr[i].profileName != nullptr)
      {
        delete[] profiles_ptr[i].profileName;
      }
      if(profiles_ptr[i].nickName != nullptr)
      {
        delete[] profiles_ptr[i].nickName;
      }
      if(profiles_ptr[i].spName != nullptr)
      {
        delete[] profiles_ptr[i].spName;
      }
    }

    delete[] profiles_ptr;
    profiles_ptr = nullptr;

  /* If request is GET profile, no need to process further */
    if (orig_ptr->event == QCRIL_UIM_LPA_GET_PROFILE)
    {
      return;
    }
  }

  if (profiles_ptr != nullptr)
  {
    qcril_uim_lpa_profiles_cached   = TRUE;
  }

  switch(orig_ptr->event)
  {
    case QCRIL_UIM_LPA_GET_PROFILE:
      lpa_rsp_ptr->profiles = profiles_ptr;
      lpa_rsp_ptr->no_of_profiles = no_of_profiles;
      result = TRUE;
      break;
    case QCRIL_UIM_LPA_ENABLE_PROFILE:
    case QCRIL_UIM_LPA_DISABLE_PROFILE:
      result = qcril_uim_lpa_enable_disable_profile(orig_ptr,
                                                    orig_ptr->iccid,
                                                    orig_ptr->event,
                                                    TRUE);
      break;
    case QCRIL_UIM_LPA_DELETE_PROFILE:
      result = qcril_uim_lpa_delete_profile(orig_ptr,
                                            orig_ptr->iccid,
                                            TRUE);
      break;
    default:
      break;
  }

  if (result)
  {
    lpa_rsp_ptr->result = QCRIL_UIM_LPA_RESULT_SUCCESS;
  }
} /* qcril_uim_lpa_get_profile_info_resp */


/*===========================================================================

                               QCRIL INTERFACE FUNCTIONS

===========================================================================*/
/*=========================================================================

  FUNCTION:  qcril_uim_lpa_cleanup_cached_iccids

===========================================================================*/
/*!
    @brief
    Resets the cached iccids.

    @return
    None
*/
/*=========================================================================*/
void qcril_uim_lpa_cleanup_cached_iccids
(
)
{
  int i = 0;

  QCRIL_LOG_DEBUG("qcril_uim_lpa_cleanup_cached_iccids");

  for (i = 0; i < QMI_UIM_PROFILES_MAX; i++)
  {
    memset(qcril_uim_lpa_profileId_iccId[i], 0xFF, QCRIL_UIM_ICCID_LEN);
  }
  qcril_uim_lpa_profiles_cached = FALSE;
}


/*=========================================================================

  FUNCTION:  qcril_uim_lpa_process_qmi_callback

===========================================================================*/
/*!
    @brief
    Handles MessageId_QCRIL_EVT_UIM_LPA_QMI_COMMAND_CALLBACK.

    @return
    None
*/
/*=========================================================================*/
void qcril_uim_lpa_process_qmi_callback
(
  qmi_uim_rsp_id_type                 msg_id,
  qcril_uim_callback_params_type    * resp_ptr,
  void                              * orig_req_ptr
)
{
  std::shared_ptr<UimLpaReqMessage>              req_ptr       = nullptr;
  std::shared_ptr<qcril_uim_lpa_response_type>   resp_data_ptr = nullptr;
  qcril_uim_lpa_user_resp_type                   lpa_user_resp;
  qcril_uim_lpa_user_req_orig_type             * orig_ptr =
         (qcril_uim_lpa_user_req_orig_type *)orig_req_ptr;

  /* Sanity checks */
  if ((resp_ptr == NULL) || (orig_ptr == NULL) || orig_ptr->req_ptr == nullptr)
  {
    QCRIL_LOG_ERROR("%s", "Invalid input, cannot process request");
    return;
  }

  req_ptr = orig_ptr->req_ptr;

  resp_data_ptr = std::shared_ptr<qcril_uim_lpa_response_type>(new qcril_uim_lpa_response_type{});

  if (resp_data_ptr == nullptr)
  {
    return;
  }

  memset(resp_data_ptr.get(), 0x00, sizeof(qcril_uim_lpa_response_type));
  memset(&lpa_user_resp, 0x00, sizeof(lpa_user_resp));

  if (resp_ptr->qmi_rsp_data.qmi_err_code == 0)
  {
    lpa_user_resp.result = QCRIL_UIM_LPA_RESULT_SUCCESS;
  }
  else
  {
    lpa_user_resp.result = QCRIL_UIM_LPA_RESULT_FAILURE;
  }

  lpa_user_resp.event = orig_ptr->event;

  switch(msg_id)
  {
    case QMI_UIM_SRVC_DELETE_PROFILE_RSP_MSG:
    case QMI_UIM_SRVC_EUICC_MEMORY_RESET_RSP_MSG:
    case QMI_UIM_SRVC_ADD_PROFILE_RSP_MSG:
    case QMI_UIM_SRVC_UPDATE_PROFILE_NICKNAME_RSP_MSG:
    case QMI_UIM_SRVC_SET_SIM_PROFILE_RSP_MSG:
    case QMI_UIM_SRVC_USER_CONSENT_RSP_MSG:
      break;

    case QMI_UIM_SRVC_GET_SIM_PROFILES_RSP_MSG:
      if (resp_ptr->qmi_rsp_data.qmi_err_code == 0)
      {
        qcril_uim_lpa_get_profile_info_resp(&resp_ptr->qmi_rsp_data.rsp_data.get_profiles_resp,
                                            &lpa_user_resp,
                                            orig_ptr);
      }
      if (lpa_user_resp.event != QCRIL_UIM_LPA_GET_PROFILE &&
          lpa_user_resp.result != QCRIL_UIM_LPA_RESULT_SUCCESS)
      {
        /* Skip sending response to LPA module GET profile request is because of other request */
        return;
      }
      break;

    case QMI_UIM_SRVC_GET_EID_RSP_MSG:
      lpa_user_resp.eid_len = resp_ptr->qmi_rsp_data.rsp_data.eid_resp.eid_len;
      lpa_user_resp.eid = resp_ptr->qmi_rsp_data.rsp_data.eid_resp.eid_ptr;
      break;

    case QMI_UIM_SRVC_GET_SET_SERVER_ADDRESS_RSP_MSG:
      lpa_user_resp.srvAddr.smdpAddress =
        resp_ptr->qmi_rsp_data.rsp_data.euicc_server_address_rsp.smdp_address_ptr;
      lpa_user_resp.srvAddr.smdsAddress =
        resp_ptr->qmi_rsp_data.rsp_data.euicc_server_address_rsp.smds_address_ptr;
      break;

    default:
      /* This shouldn't happen since we never post for these msg ids */
      QCRIL_LOG_ERROR("Unsupported QMI UIM LPA response: 0x%x",
                      msg_id);
      break;
  }

  resp_data_ptr->resp_id = UIM_LPA_USER_RESP_ID;
  resp_data_ptr->data_ptr = &lpa_user_resp;

  req_ptr->sendResponse(req_ptr, Message::Callback::Status::SUCCESS, resp_data_ptr);

  if (lpa_user_resp.profiles != nullptr)
  {
    uint8_t  i = 0;

    for (i = 0; i < lpa_user_resp.no_of_profiles && i < QMI_UIM_PROFILES_MAX; i++)
    {
      if(lpa_user_resp.profiles[i].iccid != nullptr)
      {
        delete[] lpa_user_resp.profiles[i].iccid;
      }
      if(lpa_user_resp.profiles[i].icon != nullptr)
      {
        delete[] lpa_user_resp.profiles[i].icon;
      }
      if(lpa_user_resp.profiles[i].profileName != nullptr)
      {
        delete[] lpa_user_resp.profiles[i].profileName;
      }
      if(lpa_user_resp.profiles[i].nickName != nullptr)
      {
        delete[] lpa_user_resp.profiles[i].nickName;
      }
      if(lpa_user_resp.profiles[i].spName != nullptr)
      {
        delete[] lpa_user_resp.profiles[i].spName;
      }
    }

    delete[] lpa_user_resp.profiles;
  }

  orig_ptr->req_ptr = nullptr;
  delete orig_ptr;
} /* qcril_uim_lpa_process_qmi_callback */


/*===========================================================================

  FUNCTION:  qcril_uim_lpa_add_profile_ind

===========================================================================*/
void qcril_uim_lpa_add_profile_ind
(
  const qcril_uim_indication_params_type  * ind_param_ptr,
  qcril_request_return_type               * const ret_ptr /*!< Output parameter */
)
{
  qcril_uim_lpa_add_profile_progress_ind_type add_profile_ind;
  std::shared_ptr<UimLpaIndicationMsg>        ind_msg_ptr = nullptr;

  memset(&add_profile_ind, 0, sizeof(add_profile_ind));

  if (ind_param_ptr == NULL || ret_ptr == NULL)
  {
    QCRIL_LOG_ERROR("Null indication ptr");
    return;
  }

  switch(ind_param_ptr->ind_data.lpa_add_profile_ind.profile_download_status)
  {
    case QMI_UIM_PROFILE_DOWNLOAD_ERROR:
      add_profile_ind.status = QCRIL_UIM_LPA_ADD_PROFILE_STATUS_ERROR;
      break;
    case QMI_UIM_PROFILE_DOWNLOAD_IN_PROGRESS:
      add_profile_ind.status = QCRIL_UIM_LPA_ADD_PROFILE_STATUS_DOWNLOAD_PROGRESS;
      break;
    case QMI_UIM_PROFILE_DOWNLOAD_COMPLETE_INSTALLATION_IN_PROGRESS:
      add_profile_ind.status = QCRIL_UIM_LPA_ADD_PROFILE_STATUS_INSTALLATION_PROGRESS;
      break;
    case QMI_UIM_PROFILE_INSTALLATION_COMPLETE:
      add_profile_ind.status = QCRIL_UIM_LPA_ADD_PROFILE_STATUS_INSTALLATION_COMPLETE;
      break;
    case QMI_UIM_PROFILE_USER_CONSENT_REQUIRED:
      add_profile_ind.status = QCRIL_UIM_LPA_ADD_PROFILE_STATUS_GET_USER_CONSENT;

      if (ind_param_ptr->ind_data.lpa_add_profile_ind.profile_policy_rules_valid)
      {
        add_profile_ind.policyMask =
          (qcril_uim_lpa_profile_ind_policy_mask)ind_param_ptr->ind_data.lpa_add_profile_ind.profile_policy_mask;

        add_profile_ind.userConsentRequired =
          (bool)ind_param_ptr->ind_data.lpa_add_profile_ind.user_consent_needed;
      }
      break;
    default:
      return;
  }

  switch(ind_param_ptr->ind_data.lpa_add_profile_ind.error_cause)
  {
    case QMI_UIM_DOWNLOAD_ERROR_CAUSE_GENERIC:
      add_profile_ind.cause = QCRIL_UIM_LPA_ADD_PROFILE_FAILURE_CAUSE_GENERIC;
      break;
    case QMI_UIM_DOWNLOAD_ERROR_CAUSE_SIM:
      add_profile_ind.cause = QCRIL_UIM_LPA_ADD_PROFILE_FAILURE_CAUSE_SIM;
      break;
    case QMI_UIM_DOWNLOAD_ERROR_CAUSE_NETWORK:
      add_profile_ind.cause = QCRIL_UIM_LPA_ADD_PROFILE_FAILURE_CAUSE_NETWORK;
      break;
    case QMI_UIM_DOWNLOAD_ERROR_CAUSE_MEMORY:
      add_profile_ind.cause = QCRIL_UIM_LPA_ADD_PROFILE_FAILURE_CAUSE_MEMORY;
      break;
    default:
      add_profile_ind.cause = QCRIL_UIM_LPA_ADD_PROFILE_FAILURE_CAUSE_NONE;
      break;
  }

  add_profile_ind.progress  = (int32_t)ind_param_ptr->ind_data.lpa_add_profile_ind.percentage;

  ind_msg_ptr = std::make_shared<UimLpaIndicationMsg>(UIM_LPA_PROFILE_PROGRESS_IND_ID,
                                                      &add_profile_ind);

  if (ind_msg_ptr != nullptr)
  {
    ind_msg_ptr->broadcast();
  }
} /* qcril_uim_lpa_add_profile_ind */


/*===========================================================================

  FUNCTION:  qcril_uim_lpa_user_request

===========================================================================*/
/*!
    @brief
    Handles MessageId_UIM_LPA_USER_REQUEST from the client.

    @return
    boolean
*/
/*=========================================================================*/
boolean qcril_uim_lpa_user_request
(
  std::shared_ptr<UimLpaReqMessage> req_ptr
)
{
  uint8_t                                         iccid[QCRIL_UIM_ICCID_LEN] = {0xFF};
  bool                                            result                     = FALSE;
  qcril_uim_lpa_user_req_type                   * in_ptr                     = nullptr;
  qcril_uim_lpa_user_resp_type                    lpa_user_resp;
  qcril_uim_lpa_user_req_orig_type              * orig_ptr                   = nullptr;
  std::shared_ptr<qcril_uim_lpa_response_type>    resp_data_ptr              = nullptr;

  if (req_ptr == nullptr)
  {
    return FALSE;
  }

  /* Sanity check */
  if(req_ptr->get_req_id() != UIM_LPA_USER_REQUEST_ID ||
     req_ptr->get_data_ptr() == nullptr)
  {
    QCRIL_LOG_ERROR("Invalid input, cannot process response");
    return FALSE;
  }

  resp_data_ptr = std::shared_ptr<qcril_uim_lpa_response_type>(new qcril_uim_lpa_response_type{});

  in_ptr = (qcril_uim_lpa_user_req_type *)req_ptr->get_data_ptr();

  memset(&lpa_user_resp, 0x00, sizeof(lpa_user_resp));
  lpa_user_resp.event = in_ptr->event;

  if (resp_data_ptr != nullptr)
  {
    resp_data_ptr->resp_id = UIM_LPA_USER_RESP_ID;
    resp_data_ptr->data_ptr = &lpa_user_resp;
  }

  orig_ptr = new qcril_uim_lpa_user_req_orig_type;

  if (orig_ptr == nullptr)
  {
    QCRIL_LOG_ERROR("Failed to allocate memory to orig_ptr");

    lpa_user_resp.result = QCRIL_UIM_LPA_RESULT_FAILURE;
    req_ptr->sendResponse(req_ptr, Message::Callback::Status::FAILURE, resp_data_ptr);

    return TRUE;
  }

  orig_ptr->event = in_ptr->event;
  orig_ptr->req_ptr = req_ptr;
  memset(orig_ptr->iccid, 0xFF, sizeof(orig_ptr->iccid));

  /* Parse input info */
  QCRIL_LOG_INFO("qcril_uim_lpa_transaction_request event : 0x%x",
                      in_ptr->event);

  switch(in_ptr->event)
  {
    case QCRIL_UIM_LPA_ADD_PROFILE:
      if (in_ptr->activationCode != NULL && strlen(in_ptr->activationCode) != 0)
      {
        result = qcril_uim_lpa_add_profile(orig_ptr, in_ptr->activationCode, in_ptr->confirmationCode);
      }
      else
      {
        lpa_user_resp.result = QCRIL_UIM_LPA_RESULT_CONFRM_CODE_MISSING;
      }
      break;

    case QCRIL_UIM_LPA_ENABLE_PROFILE:
    case QCRIL_UIM_LPA_DISABLE_PROFILE:
      if (in_ptr->iccid != NULL && in_ptr->iccid_len != 0)
      {
        if (qcril_uim_lpa_byte_to_iccid(iccid,
                                        QCRIL_UIM_ICCID_LEN,
                                        in_ptr->iccid,
                                        in_ptr->iccid_len) > 0)
        {
          memcpy(orig_ptr->iccid, iccid, sizeof(orig_ptr->iccid));
          result = qcril_uim_lpa_enable_disable_profile(orig_ptr, iccid, in_ptr->event, FALSE);
        }
      }
      break;

    case QCRIL_UIM_LPA_DELETE_PROFILE:
      if (in_ptr->iccid != NULL && in_ptr->iccid_len != 0)
      {
        if (qcril_uim_lpa_byte_to_iccid(iccid,
                                        QCRIL_UIM_ICCID_LEN,
                                        in_ptr->iccid,
                                        in_ptr->iccid_len) > 0)
        {
          memcpy(orig_ptr->iccid, iccid, sizeof(orig_ptr->iccid));
          result = qcril_uim_lpa_delete_profile(orig_ptr, iccid, FALSE);
        }
      }
      break;

    case QCRIL_UIM_LPA_EUICC_MEMORY_RESET:
      result = qcril_uim_lpa_memory_reset(orig_ptr, in_ptr->resetMask);
      break;

    case QCRIL_UIM_LPA_GET_PROFILE:
      result = qcril_uim_lpa_get_profile(orig_ptr);
      break;

    case QCRIL_UIM_LPA_UPDATE_NICKNAME:
      if (in_ptr->iccid != NULL && in_ptr->iccid_len != 0)
      {
        if (qcril_uim_lpa_byte_to_iccid(iccid,
                                        QCRIL_UIM_ICCID_LEN,
                                        in_ptr->iccid,
                                        in_ptr->iccid_len) > 0)
        {
          memcpy(orig_ptr->iccid, iccid, sizeof(orig_ptr->iccid));
          result = qcril_uim_lpa_update_nickname(orig_ptr, iccid, in_ptr->nickname);
        }
      }
      break;

    case QCRIL_UIM_LPA_GET_EID:
      result = qcril_uim_lpa_get_eid(orig_ptr);
      break;

    case QCRIL_UIM_LPA_USER_CONSENT:
      result = qcril_uim_lpa_user_consent(orig_ptr, in_ptr->userOk);
      break;

    case QCRIL_UIM_LPA_SRV_ADDR_OPERATION:
      result = qcril_uim_lpa_get_set_server_address(orig_ptr, in_ptr->srvOpReq.smdpAddress, in_ptr->srvOpReq.opCode);
      break;

    default:
      break;
  }

  if (!result)
  {
    if (lpa_user_resp.result == QCRIL_UIM_LPA_RESULT_SUCCESS)
    {
      lpa_user_resp.result = QCRIL_UIM_LPA_RESULT_FAILURE;
    }

    /* In any case of error, send response back to client */
    QCRIL_LOG_ERROR("Error in qcril_uim_lpa_user_request, result: 0x%x", result);
    req_ptr->sendResponse(req_ptr, Message::Callback::Status::FAILURE, resp_data_ptr);
  }

  return TRUE;
} /* qcril_uim_lpa_user_request */
