/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#include "modules/lpa/LpaUimHttpTransactionRspMsg.h"


LpaUimHttpTransactionRspMsg::LpaUimHttpTransactionRspMsg
(
  uint32_t dataSize,
  void *dataPtr,
  unsigned long msg_id,
  int tansp_err
) :SolicitedMessage<void>(get_class_message_id())
{
  mMsgPtr = nullptr;
  mMsgLen = 0;

  if (dataPtr != nullptr)
  {
    mMsgPtr = new uint8_t[dataSize];

    if (mMsgPtr != nullptr)
    {
      memcpy(mMsgPtr, dataPtr, dataSize);
      mMsgLen = dataSize;
    }
  }
  mTranspErr = tansp_err;
  mMsgId = msg_id;
} /* LpaUimHttpTransactionRspMsg::LpaUimHttpTransactionRspMsg */


LpaUimHttpTransactionRspMsg::~LpaUimHttpTransactionRspMsg()
{
  if (mMsgPtr != nullptr)
  {
    delete[] mMsgPtr;
    mMsgPtr = nullptr;
  }
  mMsgLen = 0;
} /* LpaUimHttpTransactionRspMsg::~LpaUimHttpTransactionRspMsg */
