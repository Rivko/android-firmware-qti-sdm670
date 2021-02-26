/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#pragma once
#include "framework/SolicitedMessage.h"
#include "framework/add_message_id.h"
#include "framework/Dispatcher.h"
#include "telephony/ril.h"

/*
* In general below is the type-safety measure provided by framework.
* "Message declaration outlines the expected TYPE in callback".
*
* Represented roughly with below diagram as follow...
* GenericMessage<T> <=> GenericCallback<T>
*
* this association is enforced by framework during initialization of Message
*object.
* You would not be able to set a callback which does not take template type T as
*mandated
* by Message declaration.
*
* In below sample, the message SampleMessage was derived from
*GenericMessage<string> so the
* template type was string and the SampleMessage constructor would
*mandate/require a callback derived
* from GenericCallback<string>.
* An attempt to set any other callback(taking other tempated type) would result
*in compilation error.
* Also msg->sendResponse() is also template so module has to provide only the
*templated type-safe value while
* responding back.
*
*/
class SampleMessage : public SolicitedMessage<string>,
                      public add_message_id<SampleMessage>
{
  RIL_Dial *mDialReqPtr;

 public:
  static constexpr const char *MESSAGE_NAME = "SOME_SAMPLE_MESSAGE";

  inline SampleMessage(size_t dataSize, RIL_Dial *dataPtr,
                       GenericCallback<string> *callback):
      SolicitedMessage<string>(get_class_message_id())
  {
    (void)dataSize;
    mName = MESSAGE_NAME;
    deepCopy((RIL_Dial *)dataPtr);
    setCallback(callback);
  }
  ~SampleMessage();

  /* You can choose tp provide convenient method to access telephony data
  * structure*/
  string getNumber();
  /* Or you can opt to get the telephony data structure directly for you legacy
  * handlers sake.*/
  RIL_Dial *getData();

  string dump();

 private:
  void deepCopy(RIL_Dial const *dialReqPtr);
};
