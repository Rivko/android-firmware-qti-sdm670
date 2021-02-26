
/*===========================================================================

  Copyright (c) 2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

===========================================================================*/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#if defined (FEATURE_QCRIL_UIM_QMI)

#include "framework/Log.h"
#include "modules/uim/qcril_uim_legacy.h"
#include "modules/uim/qcril_uim.h"
#include "qcril_uim_card.h"
#include "qcril_uim_util.h"
#include "qcril_uim_restart.h"
#include "qcril_uim_file.h"


/*===========================================================================

                           DEFINES

===========================================================================*/
/* Duration to wait (in microseconds) before attempting to read ICCID again */
#define QCRIL_UIM_WAIT_FOR_DEVICE_READY 200000

/* Maximum number of attempts to read EF-ICCID on modem restart */
#define QCRIL_UIM_ICCID_READ_ATTEMPTS   10


/*=========================================================================

  FUNCTION:  qcril_uim_verify_pin_on_restart

===========================================================================*/
/*!
    @brief
    Checks ICCID and AID before sending PIN1 for verification on modem
    restart.

    @return
    Status of PIN1 verification.
*/
/*=========================================================================*/
static RIL_Errno qcril_uim_verify_pin_on_restart
(
  uint8                                        slot,
  uint8                                        app_index,
  uint8                                        pin_index
)
{
  qmi_uim_rsp_data_type                     pin_rsp_data;
  qmi_uim_verify_pin_params_type            pin_params;
  int                                       res       = 0;
  uint16                                    slot_app  = 0;
  uint8                                     pin1_data[QMI_UIM_MAX_ENCRYPTED_PIN_LEN];

  QCRIL_LOG_INFO( "%s\n", __FUNCTION__);

  memset(&pin_rsp_data, 0, sizeof(qmi_uim_rsp_data_type));
  memset(&pin_params, 0, sizeof(qmi_uim_verify_pin_params_type));
  memset(pin1_data, 0, sizeof(pin1_data));

  if(pin_index >= QCRIL_UIM_MAX_ENCRYPTED_PIN_INFO ||
     qcril_uim.pin1_info[pin_index].slot != slot)
  {
    QCRIL_LOG_ERROR("Invalid index for PIN1 cache: 0x%x",pin_index);
    return QCRIL_UIM_RIL_E_INTERNAL_ERR;
  }

  if(qcril_uim.pin1_info[pin_index].encrypted_pin1_len == 0)
  {
    QCRIL_LOG_ERROR("%s","Encrypted PIN1 not cached");
    return QCRIL_UIM_RIL_E_INTERNAL_ERR;
  }

  slot_app = slot;
  slot_app = (slot_app << 8) | app_index;
  if(slot_app == qcril_uim.card_status.index_gw_pri_prov)
  {
    pin_params.session_info.session_type = QMI_UIM_SESSION_TYPE_PRI_GW_PROV;
  }
  else if(slot_app == qcril_uim.card_status.index_1x_pri_prov)
  {
    pin_params.session_info.session_type = QMI_UIM_SESSION_TYPE_PRI_1X_PROV;
  }
  else if(slot_app == qcril_uim.card_status.index_gw_sec_prov)
  {
    pin_params.session_info.session_type = QMI_UIM_SESSION_TYPE_SEC_GW_PROV;
  }
  else if(slot_app == qcril_uim.card_status.index_1x_sec_prov)
  {
    pin_params.session_info.session_type = QMI_UIM_SESSION_TYPE_SEC_1X_PROV;
  }
  else if(slot_app == qcril_uim.card_status.index_gw_ter_prov)
  {
    pin_params.session_info.session_type = QMI_UIM_SESSION_TYPE_TER_GW_PROV;
  }
  else if(slot_app == qcril_uim.card_status.index_1x_ter_prov)
  {
    pin_params.session_info.session_type = QMI_UIM_SESSION_TYPE_TER_1X_PROV;
  }

  pin_params.pin_data.data_ptr = pin1_data;

  /* Copy the cached PIN1 value */
  memcpy(pin_params.pin_data.data_ptr, qcril_uim.pin1_info[pin_index].encrypted_pin1,
         qcril_uim.pin1_info[pin_index].encrypted_pin1_len);

  pin_params.pin_data.data_len = qcril_uim.pin1_info[pin_index].encrypted_pin1_len;
  pin_params.is_pin1_encrypted = QMI_UIM_TRUE;
  pin_params.pin_id            = QMI_UIM_PIN_ID_PIN1;

  memset(&pin_rsp_data, 0, sizeof(qmi_uim_rsp_data_type));

  /* Set the silent_pin1_verify_in_progress flag to TRUE. This flag will be
     set to FALSE either after receiving indication with PIN1 enabled and
     verified or an error in PIN1 verification. */
  qcril_uim.pin1_info[pin_index].silent_verify_in_progress = TRUE;

  /* Send verify pin request */
  QCRIL_LOG_DEBUG("%s", "QMI Verify PIN" );
  res = qcril_uim_send_qmi_sync_msg(QCRIL_UIM_REQUEST_VERIFY_PIN,
                                    &pin_params,
                                    &pin_rsp_data);

  /* Client needs to free the memory for raw data */
  if(pin_rsp_data.rsp_data.verify_pin_rsp.encr_pin_data.data_ptr != NULL)
  {
    qcril_free(pin_rsp_data.rsp_data.verify_pin_rsp.encr_pin_data.data_ptr);
  }

  if ((res < 0) || (pin_rsp_data.qmi_err_code != 0))
  {
    QCRIL_LOG_ERROR("Encrypted PIN1 verification failed for slot 0x%x and app 0x%x, qmi_err_code: 0x%x",
                    slot, app_index, pin_rsp_data.qmi_err_code);
    /* Reset PIN1 cache. Failure of PIN verification means the cached info
       is invalid */
    memset(&qcril_uim.pin1_info[pin_index], 0x00, sizeof(qcril_uim_pin1_info_type));
    return QCRIL_UIM_RIL_E_SIM_ERR;
  }

  QCRIL_LOG_DEBUG("Encrypted PIN1 verified for slot 0x%x and app 0x%x",
                    slot, app_index);

  return RIL_E_SUCCESS;
} /* qcril_uim_verify_pin_on_restart */


/*===========================================================================

  FUNCTION:  qcril_uim_is_pin_enabled_not_verified

===========================================================================*/
/*!
    @brief
    Checks if PIN1 is enabled and not verified for the given application on
    the indicated slot.

    @return
    TRUE if enabled and not verified, else FALSE
*/
/*=========================================================================*/
static boolean qcril_uim_is_pin_enabled_not_verified
(
  uint8                  slot,
  uint8                  app_index
)
{
  if((app_index >= QMI_UIM_MAX_APP_PER_CARD_COUNT) ||
     (slot >= QMI_UIM_MAX_CARD_COUNT))
  {
    QCRIL_LOG_ERROR("Invalid slot or app_index; slot: %d, app_index: %d",
                    slot, app_index);
    return FALSE;
  }

  /* Check appropriate pin state */
  if(qcril_uim.card_status.card[slot].application[app_index].univ_pin ==
     QCRIL_UIM_UPIN_STATE_REPLACES_PIN1)
  {
    if (qcril_uim.card_status.card[slot].upin_state ==
        QMI_UIM_PIN_STATE_ENABLED_NOT_VERIFIED)
    {
      QCRIL_LOG_DEBUG("UPIN is ENABLED_NOT_VERIFIED on slot %d", slot);
      return TRUE;
    }
  }
  else if(qcril_uim.card_status.card[slot].application[app_index].pin1_state ==
     QMI_UIM_PIN_STATE_ENABLED_NOT_VERIFIED)
  {
    QCRIL_LOG_DEBUG("PIN1 is ENABLED_NOT_VERIFIED on slot %d", slot);
    return TRUE;
  }

  return FALSE;
} /* qcril_uim_is_pin_enabled_not_verified */


/*===========================================================================

  FUNCTION:  qcril_uim_is_pin_verification_needed

===========================================================================*/
/*!
    @brief
    Checks if PIN1 needs to be verified for any application on the given slot.

    @return
    TRUE if verification is needed, else FALSE
*/
/*=========================================================================*/
static boolean qcril_uim_is_pin_verification_needed
(
  uint8                  slot
)
{
  uint8 app_index = 0;

  if(slot >= QMI_UIM_MAX_CARD_COUNT)
  {
    QCRIL_LOG_ERROR("Invalid slot: %d", slot);
    return FALSE;
  }

  for(app_index = 0; app_index < qcril_uim.card_status.card[slot].num_app; app_index++)
  {
    /* Check appropriate pin state */
    if(qcril_uim.card_status.card[slot].application[app_index].univ_pin ==
       QCRIL_UIM_UPIN_STATE_REPLACES_PIN1)
    {
      if (qcril_uim.card_status.card[slot].upin_state <=
          QMI_UIM_PIN_STATE_ENABLED_NOT_VERIFIED)
      {
        QCRIL_LOG_DEBUG("UPIN verification needed for slot %d and app %d",
                        slot, app_index);
        return TRUE;
      }
    }
    else if(((qcril_uim.card_status.card[slot].application[app_index].app_type == QMI_UIM_APP_SIM) ||
             (qcril_uim.card_status.card[slot].application[app_index].app_type == QMI_UIM_APP_USIM) ||
             (qcril_uim.card_status.card[slot].application[app_index].app_type == QMI_UIM_APP_RUIM) ||
             (qcril_uim.card_status.card[slot].application[app_index].app_type == QMI_UIM_APP_CSIM) ) &&
           (qcril_uim.card_status.card[slot].application[app_index].pin1_state <=
             QMI_UIM_PIN_STATE_ENABLED_NOT_VERIFIED))
    {
      QCRIL_LOG_DEBUG("PIN1 verification needed for slot %d and app %d",
                       slot, app_index);
      return TRUE;
    }
  }

  return FALSE;
} /* qcril_uim_is_pin_verification_needed */


/*===========================================================================

  FUNCTION:  qcril_uim_match_aid_in_encrypted_pin_cache

===========================================================================*/
/*!
    @brief
    Finds index of the encrypted PIN1 data in the cache that matches the
    specified AID.

    @return
    Index of the encrypted pin data if found. Size of the cache otherwise.
*/
/*=========================================================================*/
static uint8 qcril_uim_match_aid_in_encrypted_pin_cache
(
  uint8           aid_len,
  const char    * aid_ptr
)
{
  uint8 i = 0;

  for(i = 0; i < QCRIL_UIM_MAX_ENCRYPTED_PIN_INFO; i++)
  {
    if(aid_len == qcril_uim.pin1_info[i].aid_len)
    {
      if((qcril_uim.pin1_info[i].aid_len > 0) &&
         (qcril_uim.pin1_info[i].aid_len <= QMI_UIM_MAX_AID_LEN))
      {
        if((aid_ptr != NULL) &&
           (memcmp(aid_ptr,
                   qcril_uim.pin1_info[i].aid_value,
                   qcril_uim.pin1_info[i].aid_len) == 0))
        {
          return i;
        }
      }
      else if(qcril_uim.pin1_info[i].aid_len == 0)
      {
        return i;
      }
    }
  }

  return QCRIL_UIM_MAX_ENCRYPTED_PIN_INFO;
} /* qcril_uim_match_aid_in_encrypted_pin_cache */


/*===========================================================================

  FUNCTION:  qcril_uim_is_silent_pin_verification_needed

===========================================================================*/
/*!
    @brief
    Checks if pin needs to be verified silently for any application on the given slot.

    @return
    TRUE if silent pin verification is needed, else FALSE
*/
/*=========================================================================*/
boolean qcril_uim_is_silent_pin_verification_needed
(
  uint8                  slot
)
{
  uint8 app_index = 0;
  uint8 pin_index = QCRIL_UIM_MAX_ENCRYPTED_PIN_INFO;

  QCRIL_LOG_INFO( "%s\n", __FUNCTION__);

  if(slot >= QMI_UIM_MAX_CARD_COUNT)
  {
    QCRIL_LOG_ERROR("Invalid slot: %d", slot);
    return FALSE;
  }

  /* Check whether UPIN/PIN1 state is ENABLED_NOT_VERIFIED */
  for(app_index = 0; app_index < qcril_uim.card_status.card[slot].num_app; app_index++)
  {
    if(qcril_uim.card_status.card[slot].application[app_index].univ_pin ==
         QCRIL_UIM_UPIN_STATE_REPLACES_PIN1)
    {
      if (qcril_uim.card_status.card[slot].upin_state ==
            QMI_UIM_PIN_STATE_ENABLED_NOT_VERIFIED)
      {
        QCRIL_LOG_INFO("Silent UPIN verification needed for slot %d and app %d",
                        slot, app_index);
        break;
      }
    }
    else if(((qcril_uim.card_status.card[slot].application[app_index].app_type == QMI_UIM_APP_SIM) ||
             (qcril_uim.card_status.card[slot].application[app_index].app_type == QMI_UIM_APP_USIM) ||
             (qcril_uim.card_status.card[slot].application[app_index].app_type == QMI_UIM_APP_RUIM) ||
             (qcril_uim.card_status.card[slot].application[app_index].app_type == QMI_UIM_APP_CSIM) ) &&
           (qcril_uim.card_status.card[slot].application[app_index].pin1_state ==
             QMI_UIM_PIN_STATE_ENABLED_NOT_VERIFIED))
    {
      QCRIL_LOG_INFO("Silent PIN1 verification needed for slot %d and app %d",
                       slot, app_index);
      break;
    }
  }

  /* If app index is equal to number of apps then there is no such app with
     PIN1/UPIN state as ENABLED_NOT_VERIFIED */
  if(app_index == qcril_uim.card_status.card[slot].num_app)
  {
    return FALSE;
  }

  /* If the app details match then pin verification is in progress/yet to be done */
  pin_index = qcril_uim_match_aid_in_encrypted_pin_cache(
                qcril_uim.card_status.card[slot].application[app_index].aid_len,
                qcril_uim.card_status.card[slot].application[app_index].aid_value);

  if(pin_index < QCRIL_UIM_MAX_ENCRYPTED_PIN_INFO)
  {
    return TRUE;
  }

  return FALSE;
} /* qcril_uim_is_silent_pin_verification_needed */


/*===========================================================================

  FUNCTION:  qcril_uim_get_empty_encrypted_pin_index

===========================================================================*/
/*!
    @brief
    Returns first available index in the encrypted PIN cache.

    @return
    Index of the encrypted pin data if found.
*/
/*=========================================================================*/
static uint8 qcril_uim_get_empty_encrypted_pin_index
(
  void
)
{
  uint8 i = 0;

  for(i = 0; i < QCRIL_UIM_MAX_ENCRYPTED_PIN_INFO; i++)
  {
    if(qcril_uim.pin1_info[i].encrypted_pin1_len == 0)
    {
      return i;
    }
  }

  return QCRIL_UIM_MAX_ENCRYPTED_PIN_INFO;
} /* qcril_uim_get_empty_encrypted_pin_index */


/*=========================================================================

  FUNCTION:  qcril_uim_store_encrypted_pin

===========================================================================*/
/*!
    @brief
    Stores encrypted PIN1 along with ICCID and application ID.

    @return
    None.
*/
/*=========================================================================*/
void qcril_uim_store_encrypted_pin
(
  qmi_uim_rsp_data_type                        *rsp_data_ptr,
  qmi_uim_session_type                         session_type
)
{
  uint8                      slot         = QCRIL_UIM_INVALID_SLOT_INDEX_VALUE;
  uint8                      app_index    = QCRIL_UIM_INVALID_APP_INDEX_VALUE;
  uint8                      pin_index    = QCRIL_UIM_MAX_ENCRYPTED_PIN_INFO;
  int                        i            = 0;
  qmi_uim_data_type        * pin_data_ptr = NULL;
  RIL_Errno                  ril_err      = QCRIL_UIM_RIL_E_INTERNAL_ERR;
  uint8                      iccid_len    = QCRIL_UIM_ICCID_LEN;
  uint8                      iccid_data[QCRIL_UIM_ICCID_LEN];

  QCRIL_LOG_INFO( "%s\n", __FUNCTION__);

  memset(iccid_data, 0x00, sizeof(iccid_data));

  /* Determine index of the app for which PIN operation was performed */
  if(session_type == QMI_UIM_SESSION_TYPE_PRI_GW_PROV)
  {
    app_index = qcril_uim.card_status.index_gw_pri_prov & 0xFF;
    slot = (qcril_uim.card_status.index_gw_pri_prov >> 8) & 0xFF;
  }
  else if(session_type == QMI_UIM_SESSION_TYPE_PRI_1X_PROV)
  {
    app_index = qcril_uim.card_status.index_1x_pri_prov & 0xFF;
    slot = (qcril_uim.card_status.index_1x_pri_prov >> 8) & 0xFF;
  }
  else if(session_type == QMI_UIM_SESSION_TYPE_SEC_GW_PROV)
  {
    app_index = qcril_uim.card_status.index_gw_sec_prov & 0xFF;
    slot = (qcril_uim.card_status.index_gw_sec_prov >> 8) & 0xFF;
  }
  else if(session_type == QMI_UIM_SESSION_TYPE_SEC_1X_PROV)
  {
    app_index = qcril_uim.card_status.index_1x_sec_prov & 0xFF;
    slot = (qcril_uim.card_status.index_1x_sec_prov >> 8) & 0xFF;
  }
  else if(session_type == QMI_UIM_SESSION_TYPE_TER_GW_PROV)
  {
    app_index = qcril_uim.card_status.index_gw_ter_prov & 0xFF;
    slot = (qcril_uim.card_status.index_gw_ter_prov >> 8) & 0xFF;
  }
  else if(session_type == QMI_UIM_SESSION_TYPE_TER_1X_PROV)
  {
    app_index = qcril_uim.card_status.index_1x_ter_prov & 0xFF;
    slot = (qcril_uim.card_status.index_1x_ter_prov >> 8) & 0xFF;
  }
  else
  {
    QCRIL_LOG_ERROR("PIN1 caching not supported for session type: 0x%x",
                    session_type);
    return;
  }

  if((slot >= QMI_UIM_MAX_CARD_COUNT) ||
     (app_index >= QMI_UIM_MAX_APP_PER_CARD_COUNT))
  {
    QCRIL_LOG_ERROR("Invalid slot or app_index; slot: %d, app_index: %d",
                    slot, app_index);
    return;
  }

  /* Check if encrypted PIN1 was previously cached for this session type */
  pin_index = qcril_uim_match_aid_in_encrypted_pin_cache(
                qcril_uim.card_status.card[slot].application[app_index].aid_len,
                qcril_uim.card_status.card[slot].application[app_index].aid_value);

  /* If this is the first time PIN1 is being stored for this session type, find
     an empty element in encrypted PIN1 cache */
  if(pin_index >= QCRIL_UIM_MAX_ENCRYPTED_PIN_INFO)
  {
    pin_index = qcril_uim_get_empty_encrypted_pin_index();

    if(pin_index >= QCRIL_UIM_MAX_ENCRYPTED_PIN_INFO)
    {
      QCRIL_LOG_ERROR("%s", "Index not found in encrypted PIN1 cache");
      return;
    }
  }

  QCRIL_LOG_DEBUG("Found index in encrypted PIN1 cache: 0x%x", pin_index);
  memset(&qcril_uim.pin1_info[pin_index], 0x00,
         sizeof(qcril_uim.pin1_info[pin_index]));

  qcril_uim.pin1_info[pin_index].slot = slot;

  /* Store encrypted PIN1 value in global*/
  switch(rsp_data_ptr->rsp_id)
  {
    case QMI_UIM_SRVC_SET_PIN_PROTECTION_RSP_MSG:
      pin_data_ptr = &rsp_data_ptr->rsp_data.set_pin_protection_rsp.encr_pin_data;
      break;
    case QMI_UIM_SRVC_VERIFY_PIN_RSP_MSG:
      pin_data_ptr = &rsp_data_ptr->rsp_data.verify_pin_rsp.encr_pin_data;
      break;
    case QMI_UIM_SRVC_UNBLOCK_PIN_RSP_MSG:
      pin_data_ptr = &rsp_data_ptr->rsp_data.unblock_pin_rsp.encr_pin_data;
      break;
    case QMI_UIM_SRVC_CHANGE_PIN_RSP_MSG:
      pin_data_ptr = &rsp_data_ptr->rsp_data.change_pin_rsp.encr_pin_data;
      break;
    default:
      /* This case should never happen!! */
      qcril_uim.pin1_info[pin_index].encrypted_pin1_len = 0;
      memset(qcril_uim.pin1_info[pin_index].encrypted_pin1, 0x00, QMI_UIM_MAX_ENCRYPTED_PIN_LEN);
      break;
  }

  if((pin_data_ptr != NULL) &&
     (pin_data_ptr->data_len > 0) &&
     (pin_data_ptr->data_len <= QMI_UIM_MAX_ENCRYPTED_PIN_LEN))
  {
    QCRIL_LOG_DEBUG("Storing encrypted PIN1 for rsp_id 0x%x :",
                    rsp_data_ptr->rsp_id);
    qcril_uim.pin1_info[pin_index].encrypted_pin1_len = pin_data_ptr->data_len;
    memcpy(qcril_uim.pin1_info[pin_index].encrypted_pin1,
           pin_data_ptr->data_ptr,
           pin_data_ptr->data_len);

    for(i = 0; i < qcril_uim.pin1_info[pin_index].encrypted_pin1_len; i++)
    {
      QCRIL_LOG_DEBUG("0x%x", qcril_uim.pin1_info[pin_index].encrypted_pin1[i] );
    }

    qcril_uim.pin1_info[pin_index].iccid_len = QCRIL_UIM_ICCID_LEN;
    ril_err = qcril_uim_read_iccid(slot,
                                   iccid_data,
                                   &iccid_len);

    if(ril_err != RIL_E_SUCCESS)
    {
      return;
    }
    /* Copy ICCID into PIN1 cache */
    memcpy(qcril_uim.pin1_info[pin_index].iccid_data,
           iccid_data,
           iccid_len);
    qcril_uim.pin1_info[pin_index].iccid_len = iccid_len;

    /* Copy AID into PIN1 cache */
    if(qcril_uim.card_status.card[slot].application[app_index].aid_len > 0 &&
       qcril_uim.card_status.card[slot].application[app_index].aid_len <= QMI_UIM_MAX_AID_LEN)
    {
      memcpy((void *)qcril_uim.pin1_info[pin_index].aid_value,
             (void *)qcril_uim.card_status.card[slot].application[app_index].aid_value,
             qcril_uim.card_status.card[slot].application[app_index].aid_len);
    }
    qcril_uim.pin1_info[pin_index].aid_len =
      qcril_uim.card_status.card[slot].application[app_index].aid_len;

    QCRIL_LOG_DEBUG("%s\n","AID stored after successful PIN operation:");
    for(i = 0; i < qcril_uim.pin1_info[pin_index].aid_len; i++)
    {
      QCRIL_LOG_DEBUG("0x%x", qcril_uim.pin1_info[pin_index].aid_value[i]);
    }
  }
} /* qcril_uim_store_encrypted_pin */


/*=========================================================================

  FUNCTION:  qcril_uim_try_pin1_verification

===========================================================================*/
/*!
    @brief
    Checks ICCID, AID and PIN1 status before sending PIN1 for verification
    after modem restart.

    @return
    Status of PIN1 verification.
*/
/*=========================================================================*/
RIL_Errno qcril_uim_try_pin1_verification
(
  uint8                                        slot
)
{
  uint8                                  app_index     = 0;
  uint8                                  pin_index     = 0xFF;
  RIL_Errno                              verify_status = QCRIL_UIM_RIL_E_INTERNAL_ERR;
  RIL_Errno                              read_status   = QCRIL_UIM_RIL_E_INTERNAL_ERR;
  boolean                                iccid_match   = FALSE;
  uint8                                  iccid_len     = QCRIL_UIM_ICCID_LEN;
  uint8                                  iccid_data[QCRIL_UIM_ICCID_LEN];
  uint8                                  err_count     = 0;

  QCRIL_LOG_DEBUG("%s\n", __FUNCTION__);

  if(slot >= QMI_UIM_MAX_CARD_COUNT)
  {
    QCRIL_LOG_ERROR("Invalid slot id: %d", slot);
    return QCRIL_UIM_RIL_E_INTERNAL_ERR;
  }

  while(err_count < QCRIL_UIM_ICCID_READ_ATTEMPTS)
  {
    if(err_count > 0)
    {
      QCRIL_LOG_DEBUG("%s", "wait for 200ms then resend request to read iccid");
      usleep(QCRIL_UIM_WAIT_FOR_DEVICE_READY);
    }

    read_status = qcril_uim_read_iccid(slot,
                                       iccid_data,
                                       &iccid_len);
    if(read_status != RIL_E_SUCCESS && read_status != RIL_E_RADIO_NOT_AVAILABLE)
    {
      return read_status;
    }
    else if(read_status == RIL_E_SUCCESS)
    {
      break;
    }

    err_count++;
  }

  if(err_count == QCRIL_UIM_ICCID_READ_ATTEMPTS)
  {
    return QCRIL_UIM_RIL_E_SIM_ERR;
  }

  if((qcril_uim.pin1_info[0].iccid_len == iccid_len) &&
     (memcmp(qcril_uim.pin1_info[0].iccid_data,
             iccid_data,
             qcril_uim.pin1_info[0].iccid_len) == 0))
  {
    iccid_match = TRUE;
  }
  else if((qcril_uim.pin1_info[1].iccid_len == iccid_len) &&
          (memcmp(qcril_uim.pin1_info[1].iccid_data,
                  iccid_data,
                  qcril_uim.pin1_info[1].iccid_len) == 0))
  {
    iccid_match = TRUE;
  }

  if(!iccid_match)
  {
    QCRIL_LOG_ERROR( "%s: Stored ICCID did not match with card\n", __FUNCTION__);
    qcril_uim.silent_pin_verify_reqd = FALSE;
    return QCRIL_UIM_RIL_E_INTERNAL_ERR;
  }

  for(app_index = 0; app_index < qcril_uim.card_status.card[slot].num_app; app_index++)
  {
    if(!qcril_uim_is_pin_enabled_not_verified(slot, app_index))
    {
      continue;
    }

     pin_index = qcril_uim_match_aid_in_encrypted_pin_cache(
                   qcril_uim.card_status.card[slot].application[app_index].aid_len,
                   qcril_uim.card_status.card[slot].application[app_index].aid_value);

    if(pin_index >= QCRIL_UIM_MAX_ENCRYPTED_PIN_INFO)
    {
      continue;
    }

    QCRIL_LOG_DEBUG("PIN1 to be verified for index 0x%x in encrypted PIN1 cache",
                    pin_index);

    verify_status = qcril_uim_verify_pin_on_restart(slot, app_index, pin_index);

    /* Reset the flag for silent PIN1 verification to FALSE */
    qcril_uim.silent_pin_verify_reqd = FALSE;

    return verify_status;
  }

  /* Reset the flag for silent PIN1 verification to FALSE only if PIN1
     verification is not required */
  if(!qcril_uim_is_pin_verification_needed(slot))
  {
    qcril_uim.silent_pin_verify_reqd = FALSE;
    return QCRIL_UIM_RIL_E_INTERNAL_ERR;
  }

  return RIL_E_SUCCESS;
} /* qcril_uim_try_pin1_verification */


/*=========================================================================

  FUNCTION:  qcril_uim_clear_encrypted_pin

===========================================================================*/
/*!
    @brief
    Clears cached value of encrypted PIN1.

    @return
    None.
*/
/*=========================================================================*/
void qcril_uim_clear_encrypted_pin
(
  qmi_uim_session_type                         session_type
)
{
  uint8                      slot         = QCRIL_UIM_INVALID_SLOT_INDEX_VALUE;
  uint8                      app_index    = QCRIL_UIM_INVALID_APP_INDEX_VALUE;
  uint8                      pin_index    = 0xFF;

  /* Determine index of the app for which PIN operation was performed */
  if(session_type == QMI_UIM_SESSION_TYPE_PRI_GW_PROV)
  {
    app_index = qcril_uim.card_status.index_gw_pri_prov & 0xFF;
    slot = (qcril_uim.card_status.index_gw_pri_prov >> 8) & 0xFF;
  }
  else if(session_type == QMI_UIM_SESSION_TYPE_PRI_1X_PROV)
  {
    app_index = qcril_uim.card_status.index_1x_pri_prov & 0xFF;
    slot = (qcril_uim.card_status.index_1x_pri_prov >> 8) & 0xFF;
  }
  else if(session_type == QMI_UIM_SESSION_TYPE_SEC_GW_PROV)
  {
    app_index = qcril_uim.card_status.index_gw_sec_prov & 0xFF;
    slot = (qcril_uim.card_status.index_gw_sec_prov >> 8) & 0xFF;
  }
  else if(session_type == QMI_UIM_SESSION_TYPE_SEC_1X_PROV)
  {
    app_index = qcril_uim.card_status.index_1x_sec_prov & 0xFF;
    slot = (qcril_uim.card_status.index_1x_sec_prov >> 8) & 0xFF;
  }
  else if(session_type == QMI_UIM_SESSION_TYPE_TER_GW_PROV)
  {
    app_index = qcril_uim.card_status.index_gw_ter_prov & 0xFF;
    slot = (qcril_uim.card_status.index_gw_ter_prov >> 8) & 0xFF;
  }
  else if(session_type == QMI_UIM_SESSION_TYPE_TER_1X_PROV)
  {
    app_index = qcril_uim.card_status.index_1x_ter_prov & 0xFF;
    slot = (qcril_uim.card_status.index_1x_ter_prov >> 8) & 0xFF;
  }
  else
  {
    QCRIL_LOG_DEBUG("PIN1 not cached for session type: 0x%x",
                    session_type);
    return;
  }

  if((slot >= QMI_UIM_MAX_CARD_COUNT) ||
     (app_index >= QMI_UIM_MAX_APP_PER_CARD_COUNT))
  {
    QCRIL_LOG_ERROR("Invalid slot or app_index; slot: 0x%x, app_index: 0x%x",
                    slot, app_index);
    return;
  }

  pin_index = qcril_uim_match_aid_in_encrypted_pin_cache(
                qcril_uim.card_status.card[slot].application[app_index].aid_len,
                qcril_uim.card_status.card[slot].application[app_index].aid_value);

  if(pin_index >= QCRIL_UIM_MAX_ENCRYPTED_PIN_INFO ||
     qcril_uim.pin1_info[pin_index].slot != slot)
  {
    QCRIL_LOG_DEBUG("%s\n","Correct index could not be found in PIN1 cache");
    return;
  }

  QCRIL_LOG_DEBUG("Clearing encypted PIN1 from cache for index: 0x%x", pin_index);
  memset(qcril_uim.pin1_info[pin_index].encrypted_pin1, 0x00, QMI_UIM_MAX_ENCRYPTED_PIN_LEN);
  qcril_uim.pin1_info[pin_index].encrypted_pin1_len = 0;
}/* qcril_uim_clear_encrypted_pin */


/*=========================================================================

  FUNCTION:  qcril_uim_check_silent_pin_verify_in_progress

===========================================================================*/
/*!
    @brief
    Function to check if silent pin1 verification is in progress. Resets
    the flag if PIN1 state is ENABLED_VERIFIED.

    @return
    None.
*/
/*=========================================================================*/
void qcril_uim_check_silent_pin_verify_in_progress
(
  uint8                                        slot
)
{
  uint8 pin_index = 0;
  uint8 app_index = 0;

  for (pin_index = 0; pin_index < QCRIL_UIM_MAX_ENCRYPTED_PIN_INFO; pin_index++)
  {
    if(qcril_uim.pin1_info[pin_index].slot != slot ||
       qcril_uim.pin1_info[pin_index].silent_verify_in_progress == FALSE)
    {
      continue;
    }

    for (app_index = 0; app_index < qcril_uim.card_status.card[slot].num_app; app_index++)
    {
      if((qcril_uim.pin1_info[pin_index].aid_len ==
            qcril_uim.card_status.card[slot].application[app_index].aid_len) &&
           memcmp(qcril_uim.pin1_info[pin_index].aid_value,
                  qcril_uim.card_status.card[slot].application[app_index].aid_value,
                  qcril_uim.pin1_info[pin_index].aid_len) == 0 )
      {
        if (qcril_uim.card_status.card[slot].application[app_index].pin1_state ==
            QMI_UIM_PIN_STATE_ENABLED_VERIFIED)
        {
          qcril_uim.pin1_info[pin_index].silent_verify_in_progress = FALSE;
        }
      }
    }
  }
} /* qcril_uim_check_silent_pin_verify_in_progress */


/*=========================================================================

  FUNCTION:  qcril_uim_clear_encrypted_pin_after_card_reset

===========================================================================*/
/*!
    @brief
    Clears encrypted PIN1 data after card reset.

    @return
    None.
*/
/*=========================================================================*/
void qcril_uim_clear_encrypted_pin_after_card_reset
(
  uint8                                        slot
)
{
  uint8 pin_index = 0;

  for(pin_index = 0; pin_index < QCRIL_UIM_MAX_ENCRYPTED_PIN_INFO; pin_index++)
  {
    if(slot != qcril_uim.pin1_info[pin_index].slot)
    {
      continue;
    }
    QCRIL_LOG_INFO("clearing encrypted PIN1 data for index 0x%x", pin_index);
    memset(&qcril_uim.pin1_info[pin_index], 0x00, sizeof(qcril_uim_pin1_info_type));
    qcril_uim.pin1_info[pin_index].slot = QCRIL_UIM_INVALID_SLOT_INDEX_VALUE;
  }
} /* qcril_uim_clear_encrypted_pin_after_card_reset */


/*===========================================================================

                               QCRIL INTERFACE FUNCTIONS

===========================================================================*/

/*=========================================================================

  FUNCTION:  qcril_uim_process_modem_restart_start

===========================================================================*/
/*!
    @brief
    Resets state of QCRIL_UIM, sends card error events to QCRIL, and sends
    unsolicited response to Android framework for change in card status.
    Called as a result of QCRIL_EVT_INTERNAL_MMGSDI_MODEM_RESTART_START.

    @return
    None.
*/
/*=========================================================================*/
void qcril_uim_process_modem_restart_start
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type       *const ret_ptr
)
{
  qcril_uim_pin1_info_type          pin1_info[2];
  qcril_unsol_resp_params_type      unsol_resp;
  int                               slot         = 0;
  uint8                             i            = 0;

  QCRIL_LOG_INFO( "%s\n", __FUNCTION__);

  if (params_ptr == NULL)
  {
    return;
  }

  slot = qcril_uim_instance_id_to_slot(params_ptr->instance_id);

  /* Cancel pending timers */
  qcril_uim_cancel_card_state_check_timer();

  qcril_uim_cleanup_ara_m_delay_cache_info();

  qcril_uim_update_gstk_card_event(slot, FALSE);

  /* Send generic error responses for pending commands in QCRIL UIM
     queue */
  //qcril_uim_queue_cleanup();

  /* Copy data needed for PIN1 verification */
  memcpy(pin1_info, qcril_uim.pin1_info, sizeof(qcril_uim.pin1_info));

  /* Update card state */
  if(qcril_uim.card_status.index_gw_pri_prov != QCRIL_UIM_INVALID_SLOT_APP_INDEX_VALUE)
  {
    ret_ptr->pri_gw_sim_state_changed = TRUE;
    ret_ptr->next_pri_gw_sim_state = QCRIL_SIM_STATE_ABSENT;
  }
  if(qcril_uim.card_status.index_1x_pri_prov != QCRIL_UIM_INVALID_SLOT_APP_INDEX_VALUE)
  {
    ret_ptr->pri_cdma_sim_state_changed = TRUE;
    ret_ptr->next_pri_cdma_sim_state = QCRIL_SIM_STATE_ABSENT;
  }
  if(qcril_uim.card_status.index_gw_sec_prov != QCRIL_UIM_INVALID_SLOT_APP_INDEX_VALUE)
  {
    ret_ptr->sec_gw_sim_state_changed = TRUE;
    ret_ptr->next_sec_gw_sim_state = QCRIL_SIM_STATE_ABSENT;
  }
  if(qcril_uim.card_status.index_1x_sec_prov != QCRIL_UIM_INVALID_SLOT_APP_INDEX_VALUE)
  {
    ret_ptr->sec_cdma_sim_state_changed = TRUE;
    ret_ptr->next_sec_cdma_sim_state = QCRIL_SIM_STATE_ABSENT;
  }
  if(qcril_uim.card_status.index_gw_ter_prov != QCRIL_UIM_INVALID_SLOT_APP_INDEX_VALUE)
  {
    ret_ptr->ter_gw_sim_state_changed = TRUE;
    ret_ptr->next_ter_gw_sim_state = QCRIL_SIM_STATE_ABSENT;
  }
  if(qcril_uim.card_status.index_1x_ter_prov != QCRIL_UIM_INVALID_SLOT_APP_INDEX_VALUE)
  {
    ret_ptr->ter_cdma_sim_state_changed = TRUE;
    ret_ptr->next_ter_cdma_sim_state = QCRIL_SIM_STATE_ABSENT;
  }
  ret_ptr->next_modem_state = QCRIL_MODEM_STATE_UNAVAILABLE;

  if(slot < QMI_UIM_MAX_CARD_COUNT &&
     qcril_uim.card_status.card[slot].card_state == QMI_UIM_CARD_STATE_PRESENT)
  {
    qcril_uim_update_cm_card_status(params_ptr->instance_id,
                                    params_ptr->modem_id,
                                    slot,
                                    QCRIL_CARD_STATUS_DOWN);
  }

  /* Send unsolicited response to Framework */
  qcril_default_unsol_resp_params(params_ptr->instance_id,
                                  RIL_UNSOL_RESPONSE_SIM_STATUS_CHANGED,
                                  &unsol_resp);
  qcril_send_unsol_response(&unsol_resp);

  /* Reset global variables */
  qcril_uim_reset_state();

  /* Restore PIN1 data*/
  memcpy(qcril_uim.pin1_info, pin1_info, sizeof(qcril_uim.pin1_info));

  /* Reset silent_verify_in_progress flag */
  for(i = 0; i < QCRIL_UIM_MAX_ENCRYPTED_PIN_INFO; i++)
  {
    qcril_uim.pin1_info[i].silent_verify_in_progress = FALSE;
  }
}/* qcril_uim_process_modem_restart_start*/


/*=========================================================================

  FUNCTION:  qcril_uim_process_modem_restart_complete

===========================================================================*/
/*!
    @brief
    Initializes QCRIL_UIM and sets flag to indicate that the modem has
    restarted. Called as a result of
    QCRIL_EVT_INTERNAL_MMGSDI_MODEM_RESTART_COMPLETE.

    @return
    None.
*/
/*=========================================================================*/
void qcril_uim_process_modem_restart_complete
(
  const qcril_request_params_type *const params_ptr,
  qcril_request_return_type       *const ret_ptr
)
{
  QCRIL_LOG_INFO( "%s\n", __FUNCTION__);

  (void) params_ptr;
  (void) ret_ptr;

  qcril_uim.silent_pin_verify_reqd = TRUE;
}/* qcril_uim_process_modem_restart_complete*/

#endif /* defined (FEATURE_QCRIL_UIM_QMI) */
