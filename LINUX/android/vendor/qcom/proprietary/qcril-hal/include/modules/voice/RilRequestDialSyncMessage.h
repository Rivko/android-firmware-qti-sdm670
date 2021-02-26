/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#pragma once
#include "framework/GenericCallback.h"
#include "framework/SolicitedSyncMessage.h"
#include "framework/Message.h"
#include "framework/Dispatcher.h"
#include "framework/add_message_id.h"
#include "telephony/ril.h"
#include "framework/message_translator.h"

class RilRequestDialSyncMessage : public SolicitedSyncMessage<string>,
                              public add_message_id<RilRequestDialSyncMessage>
{
 private:
  RIL_Dial *mDialReqPtr;

 public:
  static constexpr const char *MESSAGE_NAME = "RIL_REQUEST_DIAL_SYNC";

  RilRequestDialSyncMessage() = delete;
  ~RilRequestDialSyncMessage();

  inline RilRequestDialSyncMessage(size_t dataSize, RIL_Dial *dataPtr): SolicitedSyncMessage<string>(get_class_message_id()) {
    (void)dataSize;
    deepCopy((RIL_Dial *)dataPtr);
    mName = MESSAGE_NAME;
  }
  void deepCopy(RIL_Dial const *dialReqPtr);

  /* You can choose tp provide convenient method to access telephony data
   * structure*/
  string getNumber();
  /* Or you can opt to get the telephony data structure directly for you legacy
   * handlers sake.*/
  RIL_Dial *getData();

  string dump();
};
