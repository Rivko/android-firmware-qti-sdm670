/******************************************************************************
#  Copyright (c) 2017 Qualcomm Technologies, Inc.
#  All Rights Reserved.
#  Confidential and Proprietary - Qualcomm Technologies, Inc.
#******************************************************************************/
#pragma once
#include "framework/GenericCallback.h"

/* Below class represents a class of callbacks for any message which is derived
 *from
 * GenericMessage<String>.
 *
 * In general below is the type-safety measure provided by framework.
 * "Message declaration outlines the expected TYPE in callback".
 *
 * Represented roughly with below diagram as follow...
 * GenericMessage<T> <=> GenericCallback<T>
 *
 * this association is enforced by framework during initialization of Message
 *object.
 * You would not be able to set a callback which does not take template type T
 *as mandated
 * by Message declaration.
 *
 * In below sample, the message SampleMessage was derived from
 *GenericMessage<string> so the
 * template type was string and the SampleMessage constructor would
 *mandate/require a callback derived
 * from GenericCallback<string>.
 * An attempt to set any other callback(taking other tempated type) would result
 *in compilation error.
 *
*/
class SampleMessageCallback : public GenericCallback<string> {
 public:
  inline SampleMessageCallback(string clientToken)
      : GenericCallback(clientToken){};

  inline ~SampleMessageCallback(){};

  Message::Callback *clone() override;

  void onResponse(std::shared_ptr<Message> solicitedMsg, Status status,
                  std::shared_ptr<string> responseDataPtr) override;
};
