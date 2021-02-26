/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/

#include "modules/lpa/LpaUimHttpTransactionIndMsg.h"


LpaUimHttpTransactionIndMsg::LpaUimHttpTransactionIndMsg
(
  uint32_t dataSize,
  void *dataPtr,
  unsigned long msg_id,
  int tansp_err
) : UnSolicitedMessage(get_class_message_id())
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
} /* LpaUimHttpTransactionIndMsg::LpaUimHttpTransactionIndMsg */


LpaUimHttpTransactionIndMsg::~LpaUimHttpTransactionIndMsg()
{
  if (mMsgPtr != nullptr)
  {
    delete[] mMsgPtr;
    mMsgPtr = nullptr;
  }
  mMsgLen = 0;
} /* LpaUimHttpTransactionIndMsg::~LpaUimHttpTransactionIndMsg */
