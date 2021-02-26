/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#include <string.h>
#include "modules/uim/UimQmiUimIndicationMsg.h"
#include "modules/uim/qcril_uim_srvc.h"
#include "modules/uim/qcril_uim.h"
#include "modules/uim/qcril_uim_sap.h"
#include "modules/uim/qcril_uim_lpa.h"
#include "modules/uim/qcril_uim_gba.h"


/*---------------------------------------------------------------------------
  Internal functions
---------------------------------------------------------------------------*/
/*===========================================================================
  FUNCTION  qcril_uim_copy_indication - copy indication message
===========================================================================*/
static qcril_uim_indication_params_type* qcril_uim_copy_indication
(
  qmi_uim_indication_id_type           ind_id,
  const qmi_uim_indication_data_type * ind_data_ptr,
  const void                         * user_data,
  uint32_t                           * total_size_ptr
)
{
  qcril_uim_indication_params_type* out_ptr  = NULL;

  /* Sanity checks */
  if ((ind_data_ptr == NULL) || (total_size_ptr == NULL))
  {
    return NULL;
  }

  /* Calculate buffer size */
  *total_size_ptr = sizeof(qcril_uim_indication_params_type);

  /* Refresh indication is currently the only one that requires a deep copy */
  if(ind_id == QMI_UIM_SRVC_REFRESH_IND_MSG)
  {
    *total_size_ptr += ind_data_ptr->refresh_ind.refresh_event.num_files * sizeof(qmi_uim_refresh_file_id_type);
  }
  else if (ind_id == QMI_UIM_SRVC_SEND_APDU_IND_MSG)
  {
    *total_size_ptr += ind_data_ptr->send_apdu_ind.apdu.data_len;
  }

  /* Alocate memory to copy the indication */
  out_ptr = (qcril_uim_indication_params_type *)qcril_malloc(*total_size_ptr);
  if (out_ptr == NULL)
  {
    *total_size_ptr = 0;
    return NULL;
  }

  memset(out_ptr, 0, *total_size_ptr);

  out_ptr->ind_id = ind_id;
  memcpy(&out_ptr->ind_data, ind_data_ptr, sizeof(qmi_uim_indication_data_type));
  out_ptr->user_data = (void*)user_data;

  /* Deep copy */
  if(ind_id == QMI_UIM_SRVC_REFRESH_IND_MSG)
  {
    out_ptr->ind_data.refresh_ind.refresh_event.files_ptr = (qmi_uim_refresh_file_id_type*)&out_ptr->payload;
    memcpy(out_ptr->ind_data.refresh_ind.refresh_event.files_ptr,
           ind_data_ptr->refresh_ind.refresh_event.files_ptr,
           ind_data_ptr->refresh_ind.refresh_event.num_files * sizeof(qmi_uim_refresh_file_id_type));
  }
  else if (ind_id == QMI_UIM_SRVC_SEND_APDU_IND_MSG)
  {
    out_ptr->ind_data.send_apdu_ind.apdu.data_ptr = (unsigned char *)&out_ptr->payload;
    memcpy(out_ptr->ind_data.send_apdu_ind.apdu.data_ptr,
           ind_data_ptr->send_apdu_ind.apdu.data_ptr,
           ind_data_ptr->send_apdu_ind.apdu.data_len);
  }

  /* out_ptr will be freed after processing the event in
     qcril_uim_process_qmi_indication */

  return out_ptr;
} /* qcril_uim_copy_indication */


/*---------------------------------------------------------------------------
  Class functions
---------------------------------------------------------------------------*/
/*===========================================================================
  FUNCTION  UimQmiUimIndicationMsg - UimQmiUimIndicationMsg Destructor
===========================================================================*/
UimQmiUimIndicationMsg::~UimQmiUimIndicationMsg()
{
  if (mIndPtr != nullptr)
  {
    qcril_free(mIndPtr);
    mIndPtr   = nullptr;
  }
  mIndSize  = 0;
} /* UimQmiUimIndicationMsg::~UimQmiUimIndicationMsg */


/*===========================================================================
  FUNCTION  UimQmiUimIndicationMsg - UimQmiUimIndicationMsg Constructor
===========================================================================*/
UimQmiUimIndicationMsg::UimQmiUimIndicationMsg
(
  void                         * user_data,
  uint32_t                       ind_id,
  void                         * ind_data_ptr
) : UnSolicitedMessage(get_class_message_id())
{
  mIndPtr   = nullptr;
  mIndSize  = 0;
  mName     = MESSAGE_NAME;
  mIndId    = ind_id;

  switch(ind_id)
  {
    case QMI_UIM_SRVC_STATUS_CHANGE_IND_MSG:
    case QMI_UIM_SRVC_REFRESH_IND_MSG:
    case QMI_UIM_SRVC_SEND_APDU_IND_MSG:
    case QMI_UIM_SRVC_RECOVERY_IND_MSG:
    case QMI_UIM_SRVC_SUPPLY_VOLTAGE_IND_MSG:
    case QMI_UIM_SRVC_SIMLOCK_TEMP_UNLOCK_IND_MSG:
    case QMI_UIM_SRVC_LPA_APP_PROFILE_IND_MSG:
    case QMI_UIM_SRVC_SAP_IND_MSG:
    case QMI_UIM_SRVC_SLOT_STATUS_CHANGE_IND_MSG:
      mIndPtr   = (void *)qcril_uim_copy_indication((qmi_uim_indication_id_type)ind_id,
                                                    (qmi_uim_indication_data_type *)ind_data_ptr,
                                                    user_data,
                                                    &mIndSize);
      break;
    default:
      break;
  }
} /* UimQmiUimIndicationMsg::UimQmiUimIndicationMsg */


/*===========================================================================
  FUNCTION  UimQmiUimIndicationMsg - clone function
===========================================================================*/
std::shared_ptr<UnSolicitedMessage> UimQmiUimIndicationMsg::clone()
{
  qmi_uim_indication_data_type  *ind_ptr  = nullptr;
  qmi_uim_indication_id_type     ind_id   = QMI_UIM_SRVC_INVALID_IND_MSG;
  void                          *usr_data = nullptr;

  if (mIndPtr != nullptr)
  {
    ind_ptr  = &(((qcril_uim_indication_params_type *)mIndPtr)->ind_data);
    usr_data = ((qcril_uim_indication_params_type *)mIndPtr)->user_data;
    ind_id   = ((qcril_uim_indication_params_type *)mIndPtr)->ind_id;
  }
  else
  {
    ind_id = (qmi_uim_indication_id_type)mIndId;
  }
  return std::static_pointer_cast<UnSolicitedMessage>(std::make_shared<UimQmiUimIndicationMsg>(usr_data, (uint32_t)ind_id, (void *)ind_ptr));
} /* UimQmiUimIndicationMsg::clone() */
