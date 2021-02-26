/**
* Copyright (c) 2017 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
**/

#include "request/SetLteAttachProfileMessage.h"
#include <string.h>
#include "modules/android/ril_message_factory.h"


using namespace rildata;

SetLteAttachProfileMessage::SetLteAttachProfileMessage
(
  RIL_InitialAttachApn &req
):SolicitedMessage<RIL_Errno>(get_class_message_id())
{
  mName = MESSAGE_NAME;
  payload = NULL;
  deepCopy(req);
}

SetLteAttachProfileMessage::~SetLteAttachProfileMessage()
{
  if (mCallback) {
    delete mCallback;
    mCallback = nullptr;
  }

  if(payload) {
     if (payload->apn != nullptr) {
        delete[] payload->apn;
     }
     if (payload->protocol != nullptr) {
        delete[] payload->protocol;
     }
     if (payload->username != nullptr) {
        delete[] payload->username;
     }
     if (payload->password != nullptr) {
        delete[] payload->password;
     }
     delete payload;
     payload = nullptr;
  }
}

RIL_InitialAttachApn* SetLteAttachProfileMessage::get_params()
{
  return payload;
}

string SetLteAttachProfileMessage::dump()
{
  return mName + "{ }";
}

void SetLteAttachProfileMessage::deepCopy
(
  RIL_InitialAttachApn& params
)
{
  unsigned size;
  payload = new RIL_InitialAttachApn();

  if (!payload) {
    Log::getInstance().d("[" + mName + "]: " + "Ignoring invalid payload");
    return;
  }
  memcpy(payload, &params, sizeof(RIL_InitialAttachApn));
  if (params.apn != nullptr) {
     size = strlen(params.apn) + 1;
     payload->apn = new char[size];
     if (payload->apn != nullptr) {
         strlcpy(payload->apn, params.apn, size);
         Log::getInstance().d("[" + mName + "]: apn = " + params.apn);
     } else {
         Log::getInstance().d("[" + mName + "]: Ignoring invalid Payload APN");
     }
  }

  if (params.protocol != nullptr) {
     size = strlen(params.protocol) + 1;
     payload->protocol = new char[size];
     if (payload->protocol != nullptr) {
         strlcpy(payload->protocol, params.protocol, size);
         Log::getInstance().d("[" + mName + "]: protocol = " + params.protocol);
     } else {
         Log::getInstance().d("[" + mName + "]: Ignoring invalid Payload Protocol");
     }
  }

  if (params.username != nullptr) {
     size = strlen(params.username) + 1;
     payload->username = new char[size];
     if (payload->username != nullptr) {
         strlcpy(payload->username, params.username, size);
         Log::getInstance().d("[" + mName + "]: username = " + params.username);
     } else {
         Log::getInstance().d("[" + mName + "]: Ignoring invalid Payload username");
     }
  }

  if (params.password != nullptr) {
     size = strlen(params.password) + 1;
     payload->password = new char[size];
     if (payload->password != nullptr) {
         strlcpy(payload->password, params.password, size);
         Log::getInstance().d("[" + mName + "]: Password = " + params.password);
     } else {
         Log::getInstance().d("[" + mName + "]: Ignoring invalid Payload password");
     }
  }
}


#if (QCRIL_RIL_VERSION >= 15)
SetLteAttachProfileMessage_v15::SetLteAttachProfileMessage_v15
(
  RIL_InitialAttachApn_v15 &req
):SolicitedMessage<RIL_Errno>(get_class_message_id())
{
  mName = MESSAGE_NAME;
  payload = NULL;
  deepCopy(req);
}

SetLteAttachProfileMessage_v15::~SetLteAttachProfileMessage_v15()
{
 if (mCallback) {
   delete mCallback;
   mCallback = nullptr;
 }

 if(payload) {
    if (payload->apn != nullptr) {
        delete[] payload->apn;
    }
    if (payload->protocol != nullptr) {
        delete[] payload->protocol;
    }
    if (payload->roamingProtocol != nullptr) {
        delete[] payload->roamingProtocol;
    }
    if (payload->username != nullptr) {
        delete[] payload->username;
    }
    if (payload->password != nullptr) {
        delete[] payload->password;
    }
    if (payload->mvnoType != nullptr) {
        delete[] payload->mvnoType;
    }
    if (payload->mvnoMatchData != nullptr) {
        delete[] payload->mvnoMatchData;
    }
    delete payload;
    payload = nullptr;
 }
}

RIL_InitialAttachApn_v15* SetLteAttachProfileMessage_v15::get_params()
{
  return payload;
}

string SetLteAttachProfileMessage_v15::dump()
{
  return mName + "{ }";
}

void SetLteAttachProfileMessage_v15::deepCopy
(
  RIL_InitialAttachApn_v15& params
)
{
   unsigned size;
   payload = new RIL_InitialAttachApn_v15();
   if (!payload) {
       Log::getInstance().d("[" + mName + "]: " + "Ignoring invalid payload");
       return;
   }
   memcpy(payload, &params, sizeof(RIL_InitialAttachApn_v15));

   if (params.apn != nullptr) {
      size = strlen(params.apn) + 1;
      payload->apn = new char[size];
      if (payload->apn != nullptr) {
          strlcpy(payload->apn, params.apn, size);
          Log::getInstance().d("[" + mName + "]: apn = " + params.apn);
      } else {
          Log::getInstance().d("[" + mName + "]: Ignoring invalid  Payload APN");
      }
   }

   if (params.protocol != nullptr) {
      size = strlen(params.protocol) + 1;
      payload->protocol = new char[size];
      if(payload->protocol != nullptr) {
          strlcpy(payload->protocol, params.protocol, size);
          Log::getInstance().d("[" + mName + "]: protocol = " + params.protocol);
      } else {
          Log::getInstance().d("[" + mName + "]: Ignoring invalid Payload Protocol");
      }
   }

   if (params.roamingProtocol != nullptr) {
      size = strlen(params.roamingProtocol) + 1;
      payload->roamingProtocol = new char[size];
      if (payload->roamingProtocol != nullptr) {
          strlcpy(payload->roamingProtocol, params.roamingProtocol, size);
          Log::getInstance().d("[" + mName + "]: roamingProtocol = " + params.roamingProtocol);
      } else {
          Log::getInstance().d("[" + mName + "]: Ignoring invalid Payload RoamingProtocol");
      }
   }

   if (params.username != nullptr) {
      size = strlen(params.username) + 1;
      payload->username = new char[size];
      if (payload->username != nullptr) {
          strlcpy(payload->username, params.username, size);
          Log::getInstance().d("[" + mName + "]: Username = " + params.username);
      } else {
          Log::getInstance().d("[" + mName + "]: Ignoring invalid Payload Username");
      }
   }

   if (params.password != nullptr) {
      size = strlen(params.password) + 1;
      payload->password = new char[size];
      if (payload->password != nullptr) {
          strlcpy(payload->password, params.password, size);
          Log::getInstance().d("[" + mName + "]: Password = " + params.password);
      } else {
          Log::getInstance().d("[" + mName + "]: Ignoring invalid Payload Password");
      }
   }

   if (params.mvnoType != nullptr) {
      size = strlen(params.mvnoType) + 1;
      payload->mvnoType = new char[size];
      if (payload->mvnoType != nullptr) {
          strlcpy(payload->mvnoType, params.mvnoType, size);
          Log::getInstance().d("[" + mName + "]: MvnoType = " + params.mvnoType);
      } else {
          Log::getInstance().d("[" + mName + "]: Ignoring invalid Payload MvnoType");
      }
   }

   if (params.mvnoMatchData != nullptr) {
      size = strlen(params.mvnoMatchData) + 1;
      payload->mvnoMatchData = new char[size];
      if (payload->mvnoMatchData != nullptr) {
          strlcpy(payload->mvnoMatchData, params.mvnoMatchData, size);
          Log::getInstance().d("[" + mName + "]: mvnoMatchData = " + params.mvnoMatchData);
      } else {
          Log::getInstance().d("[" + mName + "]: Ignoring invalid Payload mvnoMatchData");
      }
   }
}
#endif /* QCRIL_RIL_VERSION >= 15 */

