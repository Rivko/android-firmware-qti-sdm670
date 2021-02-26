/*===========================================================================
  Copyright (c) 2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
===========================================================================*/
#include "modules/uim/UimModuleInterface.h"
#include "modules/uim/UimGetImsiRequestSyncMsg.h"
#include "modules/uim/UimGetCardStatusRequestSyncMsg.h"
#include "modules/uim/UimGetIccIdRequestSyncMsg.h"
#include "qcril_uim_card.h"

UimModuleInterface *UimModuleInterface::mUimInterface = nullptr;

RIL_Errno UimModuleInterface::get_card_status
(
  uint8_t             slot,
  RIL_CardStatus_v6 * ril_card_status /* out ptr */
)
{
  std::shared_ptr<UimGetCardStatusRequestSyncMsg> card_status_ptr = nullptr;
  std::shared_ptr<RIL_Errno>                      respPtr         = nullptr;

  card_status_ptr = std::make_shared<UimGetCardStatusRequestSyncMsg>(slot,
                                                                     ril_card_status);

  if (card_status_ptr != nullptr)
  {
    if (card_status_ptr->dispatchSync(respPtr) == Message::Callback::Status::SUCCESS)
    {
      if (respPtr != nullptr)
      {
        return *respPtr;
      }
    }
  }

  return RIL_E_GENERIC_FAILURE;
}


RIL_Errno UimModuleInterface::get_imsi
(
  uint8_t          slot,
  char           * aid_ptr,
  char          ** imsi_out_ptr /* out ptr, UIM allocates memory */
)
{
  std::shared_ptr<UimGetImsiRequestSyncMsg> imsi_ptr = nullptr;
  std::shared_ptr<RIL_Errno>                respPtr  = nullptr;

  imsi_ptr = std::make_shared<UimGetImsiRequestSyncMsg>(slot,
                                                        aid_ptr,
                                                        imsi_out_ptr);

  if (imsi_ptr != nullptr)
  {
    if (imsi_ptr->dispatchSync(respPtr) == Message::Callback::Status::SUCCESS)
    {
      if (respPtr != nullptr)
      {
        return *respPtr;
      }
    }
  }
  return RIL_E_GENERIC_FAILURE;
}


RIL_Errno UimModuleInterface::read_iccid
(
  uint8_t            slot,
  uint8_t          * iccid_data, /* out ptr */
  uint8_t          * iccid_len   /* out ptr */
)
{
  std::shared_ptr<UimGetIccIdRequestSyncMsg> iccid_ptr = nullptr;
  std::shared_ptr<RIL_Errno>                 respPtr   = nullptr;

  iccid_ptr = std::make_shared<UimGetIccIdRequestSyncMsg>(slot,
                                                          iccid_data,
                                                          iccid_len);

  if (iccid_ptr != nullptr)
  {
    if (iccid_ptr->dispatchSync(respPtr) == Message::Callback::Status::SUCCESS)
    {
      if (respPtr != nullptr)
      {
        return *respPtr;
      }
    }
  }
  return RIL_E_GENERIC_FAILURE;
}


void get_card_status_free_aid_and_label_info
(
  RIL_CardStatus_v6 * ril_card_status
)
{
  qcril_uim_free_aid_and_label_info(ril_card_status);
}
