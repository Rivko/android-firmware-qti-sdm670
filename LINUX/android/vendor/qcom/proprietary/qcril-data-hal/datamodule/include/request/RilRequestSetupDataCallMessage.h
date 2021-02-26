/**
* Copyright (c) 2017-2018 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
**/

#pragma once
#include "framework/Message.h"
#include "framework/SolicitedMessage.h"
#include "framework/message_id.h"
#include "framework/add_message_id.h"
#include "framework/message_translator.h"
#include <modules/android/RilRequestMessage.h>
#include "legacy/qcril_data_defs.h"

namespace rildata {

/********************** Class Definitions *************************/
class RilRequestSetupDataCallMessage :
                          public SolicitedMessage<generic_callback_payload>,
                          public add_message_id<RilRequestSetupDataCallMessage>
{
private:

  legacy_request_payload params;

  //Extra delay to compensate the timegap in processing the message
  //from qcril-data module to qcril-common module
  static constexpr int MESSAGE_EXTRA_DELAY { 5 };

  //Conversion of time from seconds to milliseconds or vice-versa
  static constexpr int SEC_TO_MILLISEC_CONVERTER { 1000 };

  static constexpr int MESSAGE_EXPIRY_TIMEOUT {
         (QCRIL_DATA_PROPERTY_TIMEOUT_DEFAULT + MESSAGE_EXTRA_DELAY)
           * SEC_TO_MILLISEC_CONVERTER };

  static inline void deepCopy(qcril_request_params_type &p) {

    int n = p.datalen / sizeof(char *);
    for (int i = 0; i < n; i++)
    {
      char *str = ((char **)p.data)[i];
      if (str != NULL)
      {
        int size = strlen(str) + 1;
        ((char **)p.data)[i] = new char[size];
        strlcpy(((char **)p.data)[i], str, size);
      }
    }
  }


public:
  static constexpr const char *MESSAGE_NAME =
                     "com.qualcomm.qti.qcril.data.RIL_REQUEST_SETUP_DATA_CALL";
  RilRequestSetupDataCallMessage() = delete;
  RilRequestSetupDataCallMessage(const qcril_request_params_type &p)
  :SolicitedMessage<generic_callback_payload>(get_class_message_id(),
                                              MESSAGE_EXPIRY_TIMEOUT),
                                              params(p)
  {
     mName = MESSAGE_NAME;
    deepCopy(params.get_params());
  }
  ~RilRequestSetupDataCallMessage();

  qcril_request_params_type &get_params() {
          return params.get_params();
  }
  string dump();
};

} //namespace
